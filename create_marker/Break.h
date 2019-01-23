#pragma once
#include "Note.h"

class Break : public Note
{
public:
	Break();
	Break(const trainedBlock & b);
	~Break();

private:
	void setDefaultNote(int data);
};
