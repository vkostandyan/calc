#pragma once

#include <iostream>
#include "NodeType.hpp"
#include <vector>


struct Token
{
	std::string value;
	NodeType type;

	Token(const std::string& _value, const NodeType _type) : value(_value), type(_type) { };
	~Token() { };
};

std::vector<Token> tokenizer(const std::vector<std::string> &words);
