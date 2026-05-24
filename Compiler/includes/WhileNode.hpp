#pragma once

#include "Node.hpp"

class WhileNode : public Node
{
public:
	std::unique_ptr<Node> condition;
	std::unique_ptr<Node> body;

	WhileNode() : Node(NodeType::While) { }
	~WhileNode() = default;
};