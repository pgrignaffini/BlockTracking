#pragma once


#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

class ConfigurationManager
{
private:  
	std::unordered_map<int, std::string> conf;
	std::unordered_map<int, std::vector<int>> deps;
	std::string confFile;
	std::string depsFile;

public:

	ConfigurationManager(std::string confFile);
	ConfigurationManager(std::string conf, std::string deps);

	bool checkFile();
	void loadConf();
	void loadDeps();

	void addConf(int id, std::string audio);
	void addDep(int number, int id);


	std::unordered_map<int, std::string> getConf();
	std::unordered_map<int, std::vector<int>> getDeps();
	std::vector<int> getDepsOf(int id);
	std::string getConfFile();
	std::string getDepsFile();
	std::string getAudio(int id);

};