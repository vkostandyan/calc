#pragma once

#include "Node.hpp"

struct Param
{
	std::string type;
	std::string name;
	size_t symAddr;
};

enum class ReturnType
{
	_int,
	_void
};

class ReturnNode : public Node
{
public:
	std::unique_ptr<Node> expr;

	ReturnNode() : Node(NodeType::Ret) {}
};

class FuncNode : public Node
{
public:
	ReturnType retType;
	std::vector<Param> params;
	std::unique_ptr<BlockNode> body;
	ReturnNode *returnNode = nullptr;

	FuncNode() : Node(NodeType::Func) {}
	~FuncNode() = default;
};
