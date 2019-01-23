#include "stdafx.h"
#include "BlockParser.h"


BlockParser::BlockParser()
{
}


BlockParser::~BlockParser()
{
}

vector<Function*> BlockParser::parseFunctions(vector<Block> tracked)
{
	vector<Function*> functions;

	for (int i = 0; i < tracked.size(); ++i)
	{
		if (tracked.at(i).getType() == "function")
		{
			Function* _function = new Function(tracked.at(i));
			functions.push_back(_function);
		}

	}

	return functions;
}

vector<Variable*> BlockParser::parseVariables(vector<Block> tracked)
{
	vector<Variable*> variables;

	for (int i = 0; i < tracked.size(); ++i)
	{
		if (tracked.at(i).getType() == "variable")
		{
			Variable* _variable = new Variable(tracked.at(i));
			variables.push_back(_variable);
		}

	}

	return variables;
}

vector<Bracket*> BlockParser::parseBrackets(vector<Block> tracked)
{
	vector<Bracket*> brackets;

	for (int i = 0; i < tracked.size(); ++i)
	{
		if (tracked.at(i).getType() == "bracket")
		{
			Bracket* _bracket = new Bracket(tracked.at(i));
			brackets.push_back(_bracket);
		}

	}
	
	return brackets;
}

vector<Note*> BlockParser::parseNotes(vector<Block> tracked)
{
	vector<Note*> notes;

	for (int i = 0; i < tracked.size(); ++i)
	{
		if (tracked.at(i).getType() == "note")
		{
			Note* _note = new Note(tracked.at(i));
			notes.push_back(_note);
		}

	}

	return notes;
}

vector<Break*> BlockParser::parseBreaks(vector<Block> tracked)
{
	vector<Break*> breaks;

	for (int i = 0; i < tracked.size(); ++i)
	{
		if (tracked.at(i).getType() == "break")
		{
			Break* brk = new Break(tracked.at(i));
			breaks.push_back(brk);
		}

	}

	return breaks;
}

void BlockParser::printVector(vector<Function*> blocks)
{
	for (int i = 0; i < blocks.size(); ++i)
	{
		cout << "Block " << blocks.at(i)->getID() << ": " << blocks.at(i)->getType() << endl;
	}
}

void BlockParser::printVector(vector<Variable*> blocks)
{
	for (int i = 0; i < blocks.size(); ++i)
	{
		cout << "Block " << blocks.at(i)->getID() << ": " << blocks.at(i)->getType() << endl;
	}
}

void BlockParser::printVector(vector<Note*> blocks)
{
	for (int i = 0; i < blocks.size(); ++i)
	{
		cout << "Block " << blocks.at(i)->getID() << ": " << blocks.at(i)->getType() << endl;
	}
}

void BlockParser::printVector(vector<Bracket*> blocks)
{
	for (int i = 0; i < blocks.size(); ++i)
	{
		cout << "Block " << blocks.at(i)->getID() << ": " << blocks.at(i)->getType() << endl;
	}
}

void BlockParser::printVector(vector<Break*> blocks)
{
	for (int i = 0; i < blocks.size(); ++i)
	{
		cout << "Block " << blocks.at(i)->getID() << ": " << blocks.at(i)->getType() << endl;
	}
}
