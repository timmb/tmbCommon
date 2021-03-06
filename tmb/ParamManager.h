//
//  ParamManager.h
//
//  Created by Tim Murray-Browne on 20/02/2013.
//
//

#pragma once

#ifdef CINDER_CINDER

#include "json/json.h"
#include "cinder/params/Params.h"
#include "cinder/Cinder.h"
#include <boost/tokenizer.hpp>



namespace tmb {


class BaseParameter;

class ParamManager
{
public:
	bool load(std::string const& jsonFile);
	/// This needs to be run after load(..). It is separate from load incase
	/// you want to add further parameters based on the content of the json
	/// file (accessed using get(..)
	void setup(ci::Vec2i const& paramsSize=ci::Vec2i(250,400), ci::Vec2i const& paramsPosition=ci::Vec2i(10, 10), std::string const& paramsTitle="Settings");
	void save();
	void save(std::string const& filename);
	void snapshot();
	void update(float dt, float elapsedTime);
	void draw();
	
	void addParam(std::shared_ptr<BaseParameter> parameter);
	/// Takes ownership of \p parameter
	void addParam(BaseParameter* parameter)
	{
		addParam(std::shared_ptr<BaseParameter>(parameter));
	}
	
	/// path is slash separated list of key names, e.g.
	/// "/key1/key2".
	Json::Value get(std::string const& basepath) const;

	ci::params::InterfaceGl mParams;

private:
	std::string mJsonFile;
	Json::Value mRoot;
	ci::Vec2i mPosition;
	
	std::vector<std::shared_ptr<BaseParameter> > mParameters;
};




// To avoid having to specialise the Parameter class
bool operator>>(Json::Value const& child, float& value);
bool operator>>(Json::Value const& child, double& value);
bool operator>>(Json::Value const& child, int& value);
bool operator>>(Json::Value const& child, std::string& value);
bool operator>>(Json::Value const& child, bool& value);
bool operator>>(Json::Value const& child, unsigned char& value);
template <typename T>
Json::Value& operator<<(Json::Value& lhs, T const& rhs)
{
	lhs = rhs;
	return lhs;
}

bool operator>>(Json::Value const& child, ci::Vec3f& value);
Json::Value& operator<<(Json::Value& lhs, ci::Vec3f const& rhs);
bool operator>>(Json::Value const& child, ci::Vec2f& value);
Json::Value& operator<<(Json::Value& lhs, ci::Vec2f const& rhs);




class BaseParameter
{
public:
	// path is slash separated
	// e.g. "" or "triggers" or "triggers/my_trigger/part 1"
	BaseParameter(std::string const& name, std::string const& path="")
	: path(path)
	, name(name)
	{}
	virtual ~BaseParameter() {}
	
	virtual void setup(ci::params::InterfaceGl& params) = 0;
	virtual void writeJson(Json::Value& root) const;
	virtual bool readJson(Json::Value const& root);
	
	std::string path;
	std::string name;
	
protected:
	template <typename JsonOrConstJson>
	JsonOrConstJson& getChild(JsonOrConstJson& root) const;
	virtual void toJson(Json::Value& child) const = 0;
	virtual bool fromJson(Json::Value const& child) = 0;
};



template <typename T>
T& BaseParameter::getChild(T& root) const
{
	if (path=="")
		return root[name];
	else
		return root[path][name];
}


// Main class to represent parameters
template <typename T>
class Parameter : public BaseParameter
{
public:
	Parameter(T* value_, std::string const& name, std::string const& path)
	: BaseParameter(name, path)
	, value(value_)
	{}
	
	virtual void setup(ci::params::InterfaceGl& params)
	{
		params.addParam(path+" "+name, value, "group="+path);
	}
	
protected:
	virtual void toJson(Json::Value& child) const
	{
		child << *value;
	}
	
	virtual bool fromJson(Json::Value const& child)
	{
		T tempValue;
		if (child >> tempValue)
		{
			child >> *value;
			return true;
		}
		else
			return false;
	}
	
	T* value;
};


// Specialisations
template <>
class Parameter<ci::Vec2f> : public BaseParameter
{
public:
	Parameter(ci::Vec2f* value_, std::string const& name, std::string const& path)
		: BaseParameter(name, path)
		, value(value_)
	{}

	virtual void setup(ci::params::InterfaceGl& params)
	{
		params.addParam(path+" "+name+".x", &value->x, "group="+path);
		params.addParam(path+" "+name+".y", &value->y, "group="+path);
	}

protected:
	virtual void toJson(Json::Value& child) const
	{
		child << *value;
	}
	
	virtual bool fromJson(Json::Value const& child)
	{
		ci::Vec2f tempValue;
		if (child >> tempValue)
		{
			child >> *value;
			return true;
		}
		else
			return false;
	}

	ci::Vec2f* value;
};


class EnumParameter : public Parameter<int>
{
public:
	EnumParameter(int* value_, std::string const& name, std::vector<std::string> const& enumNames, std::string const& path);
	virtual void setup(ci::params::InterfaceGl& params);

protected:
	virtual void toJson(Json::Value& child) const;
	virtual bool fromJson(Json::Value const& child);

private:
	std::vector<std::string> mEnumNames;
};


} // namespace tmb

#endif