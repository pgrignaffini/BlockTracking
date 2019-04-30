#pragma once
#include "stdafx.h"
#include "trainedBlock.h"
#include "ConfigurationManager.h"
#include <SDL_mixer.h>

class Note : public trainedBlock
{

public:
	Note();
	~Note();

	Note(const trainedBlock & b);

	std::string getAudioFile() const;

	void setID(int _id);
	void setAudioFile(string path);
	void setUpConf(ConfigurationManager* cm);
	void loadSound(const std::string &path, int volume);

	int play(int channel) override;

private:
	
	string fileAudio;
	ConfigurationManager* config; 
	Mix_Chunk* chunk;

	
};

