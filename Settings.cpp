//
//  Settings.cpp
//
//  Created by Tim Murray-Browne on 20/02/2013.
//
//

#ifdef CINDER_CINDER

#include "Settings.h"
#include "Hud.h"
#include "Common.h"
#include "cinder/app/AppBasic.h"
#include "json/json.h"
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;
using namespace ci;
namespace tmb {

bool Settings::load(string const& filename)
{
	mJsonFile = filename;
	
	
	ifstream in(filename.c_str(), ifstream::in);
	bool success = true;
	try {
		in >> mRoot;
	}
	catch (std::runtime_error& e)
	{
		app::console() << "[Settings] Error parsing json file "<<filename<<endl;
		success = false;
	}
	if (success)
	{
		app::console() << "[Settings] Successfully loaded "<<filename<<endl;
	}
	if (!success)
	{
		app::console() << "[Settings] Problem with json file";
	}
	return success;
}

void Settings::save()
{
	save(mJsonFile);
}


void Settings::snapshot()
{
	// get date string
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
	char buf[128];
	strftime(buf, sizeof(buf), "%Y-%m-%d", now);
	
	save(mJsonFile+buf+".json");
}

void Settings::save(string const& filename)
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
		app::console() <<"[Settings] Error saving JSON settings file "<<filename<<endl;
		if (out.bad()) {
			app::console() <<"[Settings] Problem with writing file"<<endl;
		}
	}
}


void Settings::setup(Vec2i const& paramsSize, string const& paramsTitle)
{
	mParams = params::InterfaceGl(paramsTitle, paramsSize);
	mParams.addButton("Save", std::bind((void (Settings::*)())&Settings::save, this));
	mParams.addButton("Save snapshot", std::bind(&Settings::snapshot, this));
	for (int i=0; i<mParameters.size(); ++i)
	{
		mParameters[i]->readJson(mRoot);
	}
	for (auto it=mParameters.begin(); it!=mParameters.end(); ++it)
	{
		(**it).setup(mParams);
	}
}

void Settings::update(float dt, float elapsed)
{

}

Json::Value Settings::get(string const& path) const
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


void Settings::add(std::shared_ptr<BaseParameter> parameter)
{
	mParameters.push_back(parameter);
}


void Settings::draw()
{
	mParams.draw();
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

}
#endif