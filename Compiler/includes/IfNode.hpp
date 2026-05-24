#pragma once

#include "Node.hpp"

class IfNode : public Node
{
public:
	std::unique_ptr<Node> condition;
	std::unique_ptr<Node> trueBranch;
	std::unique_ptr<Node> falseBranch;

	IfNode() : Node(NodeType::If) { }
	~IfNode() = default;
};