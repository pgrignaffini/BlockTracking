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
	Note(char data);

	char getNote();
	std::string getAudioFile() const;
	
	void setNote(char data);
	void setID(int _id);
	void setAudioFile(string path);
	void setUpConf(ConfigurationManager* cm);
	void loadSound(const std::string &path, int volume);

	int play(int channel) override;

private:
	char note;
	string fileAudio;
	ConfigurationManager* config; 
	Mix_Chunk* chunk;
	
};

