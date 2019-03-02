#pragma once
#include "stdafx.h"
#include "ConfigurationManager.h"

ConfigurationManager::ConfigurationManager(std::string conf)
{
	ConfigurationManager::confFile = conf;
}

ConfigurationManager::ConfigurationManager(std::string conf, std::string deps)
{
	ConfigurationManager::confFile = conf;
	ConfigurationManager::depsFile = deps;
}

bool ConfigurationManager::checkFile()
{
	return false;
}

void ConfigurationManager::loadConf()
{
	std::ifstream in(getConfFile());
	std::string stringID;
	int id;
	std::string audio;
	
	if (!in.is_open())
	{
		std::cout << "Failed to load configuration file " << std::endl;
		return;
	}
	
	else
	{
		while (!in.eof())
		{
			getline(in, stringID, ':');
			if (stringID.size() > 0) id = std::stoi(stringID);
			getline(in, audio);
			addConf(id, audio);
		}
		
	}

	std::cout << "Configuration loaded successfully" << std::endl;

	return;
}

void ConfigurationManager::loadDeps()
{
	std::ifstream in(getDepsFile());
	std::string stringNumber;
	int number;
	std::string stringIDs;
	std::string temp;
	int id;

	if (!in.is_open())
	{
		std::cout << "Failed to load dependencies file " << std::endl;
		return;
	}

	else
	{
		while (!in.eof())
		{
			getline(in, stringNumber, ':');
			if (stringNumber.size() > 0) number = std::stoi(stringNumber);
			else continue;

			getline(in, stringIDs);
			std::stringstream ss(stringIDs);
			
			while (getline(ss, temp, '-'))
			{
				id = std::stoi(temp);
				addDep(number, id);
				//std::cout << "Adding " << id << " to " << number << std::endl;
			}

			//std::cout << "++++++++" << std::endl;
			
		}

	}

	std::cout << "Dependencies loaded successfully" << std::endl;

	return;
}

void ConfigurationManager::addConf(int id, std::string audio)
{
	ConfigurationManager::conf.insert({ id, audio });
}

void ConfigurationManager::addDep(int number, int id)
{
	ConfigurationManager::deps[number].push_back(id);
}

std::string ConfigurationManager::getConfType(int id)
{
	std::unordered_map<int, std::string>::iterator confFound;

	confFound = conf.find(id);

	if (confFound != conf.end()) return "note";
	else return "function";

}

std::unordered_map<int, std::string> ConfigurationManager::getConf()
{
	return ConfigurationManager::conf;
}

std::unordered_map<int, std::vector<int>> ConfigurationManager::getDeps()
{
	return ConfigurationManager::deps;
}

std::vector<int> ConfigurationManager::getDepsOf(int id)
{
	std::vector<int> depsOf;
	std::vector<int>::iterator found;

	for (int i = 0; i < deps.size(); i++)
	{
		if ((found = std::find(deps[i].begin(), deps[i].end(), id)) != deps[i].end())  //id found, corresponds to function i
		{
			depsOf = deps[i];
			break;
		}
	}

	return depsOf;
}

std::string ConfigurationManager::getConfFile()
{
	return ConfigurationManager::confFile;
}

std::string ConfigurationManager::getDepsFile()
{
	return ConfigurationManager::depsFile;
}

std::string ConfigurationManager::getAudio(int id) 
{
	std::string sound;
	std::unordered_map<int, std::string>::iterator found;

	found = conf.find(id);

	if (found != conf.end())
		sound = found->second;

	return sound;
}
