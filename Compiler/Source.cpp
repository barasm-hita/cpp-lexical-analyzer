#include <fstream>
#include <string>
#include "Functions.h"
#include <iostream>
using namespace std;

void main()
{
	cout << "Please enter a <.cpp> file location address or press Enter for default. (Default: D:\\input.cpp)\n";
	getline(cin, fileadrs);
	if (fileadrs == "")
		fileadrs = "D:\\input.cpp";
	ifstream input;
	ofstream output;
	input.open(fileadrs, ios::in);
	output.open(fileadrs + "_output.txt", ios::out); //Delete previous file and create an new empty one!
	output.close();
	char c = NULL;
	int state = 0;
	bool cm_closed = true;
	int col = 0, row = 1, block = 0;
	Token *token = new Token();
	string cm = "";
	while (!input.eof())
	{
		switch (state)
		{
//--------------- State 0 --------------- Start Point
		case 0:
			if (c == NULL)
			{
				c = input.get();
				col++;
			}
			if (c == ' ')
				c = NULL;
			else if (c == '\t')
			{
				col += 6;
				c = NULL;
			}
			else if (c == '\n')
			{
				c = NULL;
				row++;
				col = 0;
			}
			else if (isLetter(c) || c == '_')
			{
				state = 1;
				token = InitialToken(c, col, row, block);
			}
			else if (isDigit(c))
			{
				state = 2;
				token = InitialToken(c, col, row, block);
			}
			else if (isDelimiter(c))
			{
				state = 6;
				if (c == '{')
					block++;
				token = InitialToken(c, col, row, block);
				if (c == '}')
					block--;
			}
			else if (c == '\'')
			{
				state = 7;
				token = InitialToken(c, col, row, block);
			}
			else if (c == '\"')
			{
				state = 11;
				token = InitialToken(c, col, row, block);
			}
			else
			{
				token = InitialToken(c, col, row, block);
				if (isOperator(token->Name))
					state = 5;
				else
					state = 99;
			}
			break;
//--------------- State 1 --------------- Return KeyWord or ID
		case 1:
			c = input.get();
			col++;
			if (isLetter(c) || isDigit(c) || c == '_')
				token->Name += c;
			else
			{
				if (token->Name == "true" || token->Name == "false")
					token->Type = "Literal";
				else if (isKeyword(token->Name))
					token->Type = "KeyWord";
				else
					token->Type = "Identifier";
				PrintToken(token);
				state = 0;
			}
			break;
//--------------- State 2 --------------- Return Integer Number
		case 2:
			c = input.get();
			col++;
			if (c == '.')
				state = 3;
			else if (isDigit(c))
				token->Name += c;
			else
			{
				token->Type = "Integer Number";
				PrintToken(token);
				state = 0;
			}
			break;
//--------------- State 3 --------------- Bridge from Integer to Real
		case 3:
			c = input.get();
			col++;
			if (isDigit(c))
			{
				state = 4;
				token->Name += '.';
				token->Name += c;
			}
			else
			{
				token->Type = "Integer Number";
				PrintToken(token);
				token = InitialToken('.', col - 1, row, block);
				token->Type = "Delimiter";
				PrintToken(token);
				state = 0;
			}
			break;
//--------------- State 4 --------------- Return Real Number
		case 4:
			c = input.get();
			col++;
			if (isDigit(c))
				token->Name += c;
			else
			{
				token->Type = "Real Number";
				PrintToken(token);
				state = 0;
			}
			break;
//--------------- State 5 --------------- Return Operator
		case 5:
			c = input.get();
			col++;
			if (isOperator(token->Name + c))
			{
				state = 0;
				token->Name += c;
				token->Type = "Operator";
				PrintToken(token);
				c = NULL;
			}
			else if (token->Name + c == "//" || token->Name + c == "/*")
			{
				state = 12;
				cm_closed = false;
				token->Name += c;
				cm = token->Name;
			}
			else
			{
				token->Type = "Operator";
				PrintToken(token);
				state = 0;
			}
			break;
//--------------- State 6 --------------- Return Delimiter
		case 6:
			token->Type = "Delimiter";
			PrintToken(token);
			c = NULL;
			state = 0;
			break;
//--------------- State 7 --------------- Return Literal if ''
		case 7:
			c = input.get();
			col++;
			if (c == '\'')
			{
				state = 0;
				token->Name += c;
				token->Type = "Literal";
				PrintToken(token);
				c = NULL;
			}
			else if (c == '\\')
			{
				state = 8;
				token->Name += c;
			}
			else if (c == '\n')
			{
				state = 99;
				col = 0;
				row++;
			}
			else if (c == '\t')
			{
				state = 99;
				col += 5;
			}
			else
			{
				state = 9;
				token->Name += c;
			}
			break;
//--------------- State 8 --------------- Check double Char Literals like '\t'
		case 8:
			c = input.get();
			col++;
			if (c == '\'' || c == '\"' || c == '\\' || c == 't' || c == 'n' || c == '0') // -- \' -- \" -- \\ -- \t -- \n -- \0
			{
				state = 9;
				token->Name += c;
			}
			else if (c == '\n')
			{
				state = 99;
				col = 0;
				row++;
			}
			else if (c == '\t')
			{
				state = 99;
				col += 5;
			}
			else
			{
				state = 10;
				token->Name += c;
			}
			break;
//--------------- State 9 --------------- Return double Char Literals
		case 9:
			c = input.get();
			col++;
			if (c == '\'')
			{
				token->Name += c;
				token->Type = "Literal";
				PrintToken(token);
				state = 0;
				c = NULL;
			}
			else if (c == '\n')
			{
				state = 99;
				col = 0;
				row++;
			}
			else if (c == '\t')
			{
				state = 99;
				col += 5;
			}
			else
			{
				state = 10;
				token->Name += c;
			}
			break;
//--------------- State 10 --------------- Bridge to Fail
		case 10:
			c = input.get();
			col++;
			if (c == '\n')
			{
				state = 99;
				col = 0;
				row++;
			}
			else if (c == '\t')
				col += 5;
			else if (c == '\'')
			{
				state = 99;
				token->Name += c;
			}
			else
				token->Name += c;
			break;
//--------------- State 11 --------------- Return String Literals
		case 11:
			c = input.get();
			col++;
			if (c == '\n')
			{
				state = 99;
				col = 0;
				row++;
			}
			else if (c == '\"')
			{
				token->Name += c;
				token->Type = "Literal";
				PrintToken(token);
				state = 0;
				c = NULL;
			}
			else
				token->Name += c;
			break;
//--------------- State 12 --------------- Return Comments
		case 12:
			c = input.get();
			col++;
			if (cm == "//")
			{
				if (c == '\n' || input.eof())
				{
					token->Type = "Comment";
					PrintToken(token);
					state = 0;
					cm_closed = true;
					row++;
					col = 0;
					cm = "";
					c = NULL;
				}
				else
					token->Name += c;
			}
			else
			{
				if (c == '\n')
				{
					row++;
					col = 0;
				}
				if (!input.eof())
				{
					token->Name += c;
					if (c == '*')
					{
						c = input.get();
						col++;
						if (c == '\n')
						{
							row++;
							col = 0;
						}
						if (!input.eof())
						{
							token->Name += c;
							if (c == '/')
							{
								token->Type = "Comment";
								PrintToken(token);
								state = 0;
								cm_closed = true;
								cm = "";
								c = NULL;
							}
						}
					}
				}
			}
			break;
//--------------- FAIL State ---------------
		default:
			PrintToken(token);
			state = 0;
			c = NULL;
			break;
		}
	}
	input.close();
	if (!cm_closed)
	{
		token->Type = "Comment";
		PrintToken(token);
	}
	cout << "Output file was successfully created in Drive(D:).\n";
	cout << "Press Enter to exit.\n";
	cin.get();
}