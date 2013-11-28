//
//  ParamManager.cpp
//
//  Created by Tim Murray-Browne on 20/02/2013.
//
//

#ifndef CINDER_CINDER
#define CINDER_CINDER
#endif
#ifdef CINDER_CINDER

#include "tmb/ParamManager.h"
#include "cinder/app/AppBasic.h"
#include "json/json.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include "cinder/Utilities.h"


using namespace std;
using namespace ci;

namespace tmb
{

	bool ParamManager::load(string const& filename)
	{
		mJsonFile = filename;


		ifstream in(filename.c_str(), ifstream::in);
		bool success = true;
		try {
			in >> mRoot;
		}
		catch (std::runtime_error&)
		{
			app::console() << "[ParamManager] Error parsing json file "<<filename<<endl;
			success = false;
		}
		if (success)
		{
			app::console() << "[ParamManager] Successfully loaded "<<filename<<endl;
		}
		if (!success)
		{
			app::console() << "[ParamManager] Problem with json file" << endl;
		}
		return success;
	}

	void ParamManager::save()
	{
		save(mJsonFile);
	}


	void ParamManager::snapshot()
	{
		// get date string
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
		char buf[128];
		strftime(buf, sizeof(buf), "%Y-%m-%d", now);

		save(mJsonFile+buf+".json");
	}

	void ParamManager::save(string const& filename)
	{
		Json::Value root;
		for (auto it=mParameters.begin(); it!=mParameters.end(); ++it)
		{
			(**it).writeJson(root);
		}

		ofstream out(filename.c_str(), ofstream::out);
		try
		{
			out << root;
		}
		catch (...)
		{
			app::console() <<"[ParamManager] Error saving JSON ParamManager file "<<filename<<endl;
			if (out.bad()) {
				app::console() <<"[ParamManager] Problem with writing file"<<endl;
			}
		}
	}


	void ParamManager::setup(Vec2i const& paramsSize, ci::Vec2i const& paramsPosition, string const& paramsTitle)
	{
		mParams = params::InterfaceGl(paramsTitle, paramsSize);
		mParams.addButton("Save", std::bind((void (ParamManager::*)())&ParamManager::save, this));
		mParams.addButton("Save snapshot", std::bind(&ParamManager::snapshot, this));
		for (auto i=0u; i<mParameters.size(); ++i)
		{
			mParameters[i]->readJson(mRoot);
		}
		for (auto it=mParameters.begin(); it!=mParameters.end(); ++it)
		{
			(**it).setup(mParams);
		}
		mPosition = paramsPosition;
	}

	void ParamManager::update(float dt, float elapsed)
	{

	}

	Json::Value ParamManager::get(string const& path) const
	{
		// for now, find final /
		int s = path.find_last_of('/');
		if (s==string::npos)
			s = 0;
		string base = path.substr(0, s);
		string name = path.substr(s);
		if (base!="")
			return mRoot[base][name];
		else
			return mRoot[name];
	}


	void ParamManager::addParam(std::shared_ptr<BaseParameter> parameter)
	{
		mParameters.push_back(parameter);
	}


	void ParamManager::draw()
	{
		gl::pushModelView();
		gl::translate(mPosition);
		mParams.draw();
		gl::popModelView();
	}




	////////


	void BaseParameter::writeJson(Json::Value& root) const
	{
		Json::Value& child = getChild(root);
		toJson(child);
	}


	bool BaseParameter::readJson(Json::Value const& root)
	{
		Json::Value const& child = getChild(root);
		return fromJson(child);
	}


	///////

	bool operator>>(Json::Value const& child, float& value)
	{
		if (!child.isConvertibleTo(Json::realValue))
			return false;
		value = child.asFloat();
		return true;
	}

	bool operator>>(Json::Value const& child, int& value)
	{
		if (!child.isConvertibleTo(Json::intValue))
			return false;
		value = child.asInt();
		return true;
	}

	bool operator>>(Json::Value const& child, ci::Vec3f& value)
	{
		if (child["x"].isConvertibleTo(Json::realValue)
			&& child["y"].isConvertibleTo(Json::realValue)
			&& child["z"].isConvertibleTo(Json::realValue))
		{
			value.x = child["x"].asFloat();
			value.y = child["y"].asFloat();
			value.z = child["z"].asFloat();
			return true;
		}
		return false;
	}

	bool operator>>(Json::Value const& child, std::string& value)
	{
		if (!child.isConvertibleTo(Json::realValue))
			return false;
		value = child.asString();
		return true;
	}

	Json::Value& operator<<(Json::Value& lhs, ci::Vec3f const& rhs)
	{
		lhs["x"] = rhs.x;
		lhs["y"] = rhs.y;
		lhs["z"] = rhs.z;
		return lhs;
	}

}
#endif