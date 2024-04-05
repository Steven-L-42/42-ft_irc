#pragma once

#include <iostream>
#include <sstream>
#include <vector>

class Helper
{
public:
	static std::string itoa(int i);
	static std::vector<std::string> split_buffer(const char *buffer);
	static std::string trim_whitespace(std::string str);
	static std::vector<std::string> splitString(const std::string &str);

	static std::string trim_whitespace_jan(std::string str);
};