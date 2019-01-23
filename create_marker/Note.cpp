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

void Note::setNote(char data)
{
	Note::note = data;
}

void Note::play() 
{
	this->player.playSound(this->getNote());
	cout << "Note " << getID() << " plays" << endl;
}



void Note::setID(int _id)
{
	switch (_id)
	{
		case 38: this->setNote('a');	break;
		case 33: this->setNote('b');	break;
		case 31: this->setNote('c');	break;
		case 34: this->setNote('d');	break;
		case 29: this->setNote('e');	break;
		case 30: this->setNote('f');	break;
		
		case 41: this->setNote('a');	break;
		case 49: this->setNote('b');	break;
		case 25: this->setNote('c');	break;
		case 44: this->setNote('d');	break;
		case 32: this->setNote('e');	break;
		case 43: this->setNote('f');	break;

		default: this->setNote('z');
		break;
	}

	trainedBlock::setID(_id);
}


