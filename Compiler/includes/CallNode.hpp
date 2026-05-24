#pragma once

#include "Node.hpp"

class CallNode : public Node
{
public:
	std::string name;
	std::vector<std::unique_ptr<Node>> args;

	CallNode() : Node(NodeType::Call) { }
	~CallNode() = default;
};