#pragma once
#include "trainedBlock.h"
#include "SoundManager.h"
#include "ConfigurationManager.h"

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

	void play() override;

private:
	char note;
	string fileAudio;
	SoundManager player;
	ConfigurationManager* config; 
};

