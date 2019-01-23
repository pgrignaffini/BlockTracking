#include "stdafx.h"
#include "Break.h"


Break::Break()
{
	setDefaultNote(0);
}

Break::Break(const trainedBlock &b) : Note(b)
{          

}


Break::~Break()
{
}

void Break::setDefaultNote(int data)
{
	Break::setNote(data);
}

