#pragma once

enum class Operator
{
	Add,
	Sub,
	Div,
	Mult,
};

enum class NodeType
{
	Num,
	Var,
	EofEx,
	OpBr,		// (
	ClBr,		// )
	Op,			// + - * /
	If,
	While,
	Else,
	Comp,		// > < >= <= == !=
	Assign,		// =
	Not,		// !
	Semi,		// ;
	OpBody,		// {
	ClBody,		// }
	Type,		// int, void
	Block,		// {}
	Ret,		// return
	Func,
	Call,		// f()
	Comma,		// ,
};