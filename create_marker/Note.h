#pragma once
#include "trainedBlock.h"
#include "SoundManager.h"

class Note : public trainedBlock
{

public:
	Note();
	~Note();

	Note(const trainedBlock & b);
	Note(char data);

	char getNote();
	void setNote(char data);
	void play() override;

	void setID(int _id);

private:
	char note;
	SoundManager player;
};

