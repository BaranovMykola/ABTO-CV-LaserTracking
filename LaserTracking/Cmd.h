#pragma once
#include <string>

class Cmd
{
public:
	Cmd();
	~Cmd();
	
	static std::string ask();
};

