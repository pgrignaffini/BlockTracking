#include "stdafx.h"
#include "Note.h"


Note::Note()
{

}


Note::~Note()
{
}

Note::Note(const trainedBlock &b) : trainedBlock(b)
{
	Note::setID(b.getID());
}

Note::Note(char data)
{
	this->setNote(data);
}

char Note::getNote()
{
	return Note::note;
}

std::string Note::getAudioFile() const
{
	return Note::fileAudio;
}

void Note::setNote(char data)
{
	Note::note = data;
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
	}

}


void Note::play() 
{
	this->player.playSound(this->getAudioFile());
	cout << "Note " << getID() << " plays " << this->getAudioFile() << endl;
}



void Note::setID(int _id)
{
	
	/*
	switch (_id)
	{
		case 38: this->setNote('a');	break;
		case 33: this->setNote('b');	break;
		case 31: this->setNote('c');	break;
		case 34: this->setNote('d');	break;
		case 29: this->setNote('e');	break;
		case 30: this->setNote('f');	break;
		case 26: this->setNote('g');	break;
		
		case 41: this->setNote('a');	break;
		case 49: this->setNote('b');	break;
		case 25: this->setNote('c');	break;
		case 44: this->setNote('d');	break;
		case 32: this->setNote('e');	break;
		case 47: this->setNote('e');	break;
		case 43: this->setNote('f');	break;
		case 42: this->setNote('g');	break;

		default: this->setNote('z');    break;
	}

	switch (this->getNote())
	{
		case 'a': file = "res/sounds/PianoNotes/a1.wav"; break;
		case 'b': file = "res/sounds/PianoNotes/b1.wav"; break;
		case 'c': file = "res/sounds/PianoNotes/c1.wav"; break;
		case 'd': file = "res/sounds/PianoNotes/d1.wav"; break;
		case 'e': file = "res/sounds/PianoNotes/e1.wav"; break;
		case 'f': file = "res/sounds/PianoNotes/f1.wav"; break;
		case 'g': file = "res/sounds/PianoNotes/g1.wav"; break;

		default: file = "res/sounds/error.wav"; break;
	}*/

	trainedBlock::setID(_id);
	
}



