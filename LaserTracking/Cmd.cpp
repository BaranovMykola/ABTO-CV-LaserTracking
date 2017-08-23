#include "Cmd.h"

#include <iostream>

using namespace std;

Cmd::Cmd()
{
}


Cmd::~Cmd()
{
}

string Cmd::ask()
{
	string command;
	cin >> command;
	return command;
}
