#pragma once

#include "Function.h"
#include "Variable.h"
#include "Bracket.h"
#include "Note.h"
#include "Break.h"

class BlockParser
{
public:
	BlockParser();
	~BlockParser();

	vector<Function*> parseFunctions(vector<Block> tracked);
	vector<Variable*> parseVariables(vector<Block> tracked);
	vector<Bracket*> parseBrackets(vector<Block> tracked);
	vector<Note*> parseNotes(vector<Block> tracked);
	vector<Break*> parseBreaks(vector<Block> tracked);

	void printVector(vector<Function*> blocks);
	void printVector(vector<Variable*> blocks);
	void printVector(vector<Note*> blocks);
	void printVector(vector<Bracket*> blocks);
	void printVector(vector<Break*> blocks);


};

