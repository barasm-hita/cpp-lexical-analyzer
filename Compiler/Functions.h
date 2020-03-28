#pragma once
#include <fstream>
#include <string>
using namespace std;

string fileadrs;

struct Token
{
	string Name = "";
	string Type;
	int Col;
	int Row;
	int Block;
};

inline void PrintToken(Token *t)
{
	fstream pt;
	pt.open(fileadrs + "_output.txt", ios::app);
	pt << "Name:\t" << t->Name << endl;
	pt << "Type:\t" << t->Type << endl;
	pt << "Column:\t" << t->Col << endl;
	pt << "Row:\t" << t->Row << endl;
	pt << "Block:\t" << t->Block << endl;
	pt << "------------------------------\n";
	pt.close();
}

inline Token* InitialToken(char c, int col, int row, int block)
{
	Token *t = new Token();
	t->Name += c;
	t->Type = "FAIL!!!";
	t->Col = col;
	t->Row = row;
	t->Block = block;
	return t;
}

inline bool isKeyword(string s)
{
	ifstream kw;
	kw.open("keywords.txt", ios::in);
	while (!kw.eof())
	{
		string s1;
		getline(kw, s1);
		if (s1 == s)
			return true;
	}
	kw.close();
	return false;
}

inline bool isOperator(string s)
{
	ifstream op;
	op.open("operators.txt", ios::in);
	while (!op.eof())
	{
		string s1;
		getline(op, s1);
		if (s1 == s)
			return true;
	}
	op.close();
	return false;
}

inline bool isDelimiter(char c)
{
	switch (c)
	{
	case ':':
	case ';':
	case ',':
	case '.':
	case '(':
	case ')':
	case '{':
	case '}':
	case '[':
	case ']':
		return true;
	default:
		return false;
	}
}

inline bool isLetter(char c)
{
	if (c >= 'a' && c <= 'z')
		return true;
	if (c >= 'A' && c <= 'Z')
		return true;
	return false;
}

inline bool isDigit(char c)
{
	if (c >= '0' && c <= '9')
		return true;
	return false;
}