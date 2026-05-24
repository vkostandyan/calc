#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace compiler {

enum class TokKind { Num, Ident, Plus, Minus, Mul, Div, Assign, LParen, RParen, End, Unknown };

struct Tok {
    TokKind kind = TokKind::Unknown;
    double value = 0.0;
    std::string text;
};

struct Node {
    virtual ~Node() = default;
};

using NodePtr = std::unique_ptr<Node>;

Tok make_number(const std::string& s);
std::vector<Tok> lex(std::stringstream& in);
NodePtr parse_expression(std::vector<Tok>& toks, size_t& pos);
NodePtr parse_statement(std::vector<Tok>& toks, size_t& pos);
double run(NodePtr& root);

// small convenience REPL
int repl();

} // namespace compiler
#pragma once

#include "NodeType.hpp"
#include <cstdint>
#include "IfNode.hpp"
#include "WhileNode.hpp"
#include "BlockNode.hpp"
#include <fstream>
#include "SymbolTable.hpp"
#include "Token.hpp"
#include <iomanip>
#include "FuncNode.hpp"
#include <unordered_set>
#include "CallNode.hpp"
#include "Memory.hpp"
#include "VM.hpp"

std::vector<std::string> lexer(std::stringstream& line);
std::vector<std::unique_ptr<Node>> parser(std::vector<Token>& tokens, SymbolTable& ST, int32_t& pos);
std::unique_ptr<Node> parseBlock(std::vector<Token>& tokens, SymbolTable& ST, int32_t& pos);
std::unique_ptr<Node> parseStatement(std::vector<Token>& tokens, SymbolTable& ST, int32_t& pos);
void printNode(const Token& node);
void printAST(Node* node);


enum class State
{
	Start,
	Wait_operator,
	Wait_operand,
	Error,
	End
};

inline State FSM[3][6] =
{
	// Start
	{ State::Wait_operator, State::Wait_operator, State::Error, State::Wait_operand, State::Error, State::Error },

	// Wait_operator
	{ State::Error, State::Error, State::End, State::Error, State::Wait_operator, State::Wait_operand },

	// Wait_operand
	{ State::Wait_operator, State::Wait_operator, State::Error, State::Wait_operand, State::Error, State::Error }
};

enum class OpCode : uint8_t
{
	LOAD_NUM = 0,
	LOAD_VAR,
	STORE_VAR,

	ADD,
	SUB,
	MUL,
	DIV,

	JMP,
	CMP,
	JE,
	JNE,
	JG,
	JL,
	JGE,
	JLE,

	CALL,
	RET,

	PUSH,
	POP,
	EXIT
};

struct Instruction
{
	uint8_t op;
	uint8_t dest;
	uint8_t left;
	uint8_t right;
};
