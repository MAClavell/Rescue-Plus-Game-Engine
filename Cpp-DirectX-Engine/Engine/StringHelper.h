#pragma once
#include <string>
#include <regex>


// --------------------------------------------------------
// Trim whitespace off the left of a string
// --------------------------------------------------------
std::string ltrim(const std::string& s)
{
	return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
}

// --------------------------------------------------------
// Trim whitespace off the right of a string
// --------------------------------------------------------
std::string rtrim(const std::string& s)
{
	return std::regex_replace(s, std::regex("^\\s+$"), std::string(""));
}

// --------------------------------------------------------
// Trim whitespace off both sides of a string
// --------------------------------------------------------
std::string trim(const std::string& s)
{
	return ltrim(rtrim(s));
}