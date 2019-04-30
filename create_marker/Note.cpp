#pragma once
#include "stdafx.h"
#include "Note.h"

Note::Note() : trainedBlock()
{
	fileAudio = std::string();
	config = new ConfigurationManager(fileAudio);
	chunk = new Mix_Chunk();
}


Note::~Note()
{
}

Note::Note(const trainedBlock &b) : trainedBlock(b)
{
	fileAudio = std::string();
	config = new ConfigurationManager(fileAudio);
	chunk = new Mix_Chunk();
}

std::string Note::getAudioFile() const
{
	return Note::fileAudio;
}

void Note::setAudioFile(string path)
{
	Note::fileAudio = path;
}

void Note::setUpConf(ConfigurationManager * cm)
{
	Note::config = cm;
	
	std::string file;

	file = config->getAudio(getID());
	
	if (file.size() > 0)
	{
		setAudioFile(file);
		loadSound(file, 50);
	}

}

void Note::loadSound(const std::string &path, int volume) 
{	
	this->chunk = Mix_LoadWAV(path.c_str());

	if (!chunk) 
	{
		//LOG("Couldn't load audio sample: ", path);
	}

	else Mix_VolumeChunk(chunk, volume);
}

int Note::play(int channel)
{	
	//if (channel == -1) pick the first free unreserved channel
	
	Mix_PlayChannel(channel, chunk, 0); 

	cout << "Note " << getID() << " plays " << this->getAudioFile() << " on channel " << channel << endl;
	
	return channel;
}

void Note::setID(int _id)
{
	trainedBlock::setID(_id);
}



