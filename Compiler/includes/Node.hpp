#pragma once

#include "NodeType.hpp"
#include <stack>
#include "Token.hpp"
#include <sstream>
#include <memory>

class Node
{
public:
	NodeType type;
	std::string name;

	std::unique_ptr<Node> left;
	std::unique_ptr<Node> right;

	int32_t value = 0;
	size_t symAddr = 0;
	Operator op;

public:
	Node(NodeType t) : type(t) {}

	Node(const Token& token)
	{
		type = token.type;
		name = token.value;

		switch (token.type)
		{
			case NodeType::Num:
				value = std::atoi(token.value.c_str());
				break;
			case NodeType::Var:
				name = token.value;
				break;
			case NodeType::Op:
				if (token.value == "+") op = Operator::Add;
				else if (token.value == "-") op = Operator::Sub;
				else if (token.value == "*") op = Operator::Mult;
				else if (token.value == "/") op = Operator::Div;
				break;
			default:
				break;
		}
	}
	virtual ~Node() = default;
};
