
#include "../includes/Compiler.hpp"
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <iostream>

namespace compiler {

struct NumberNode : Node { double value; NumberNode(double v):value(v){} };
struct VarNode : Node { std::string name; VarNode(std::string n):name(std::move(n)){} };
struct BinaryNode : Node { char op; NodePtr left, right; BinaryNode(char o, NodePtr l, NodePtr r):op(o),left(std::move(l)),right(std::move(r)){} };
struct AssignNode : Node { std::string name; NodePtr expr; AssignNode(std::string n, NodePtr e):name(std::move(n)),expr(std::move(e)){} };

static NodePtr parse_primary(std::vector<Tok>& toks, size_t& pos) {
	Tok& t = toks[pos];
	if (t.kind == TokKind::Num) { auto n = std::make_unique<NumberNode>(t.value); ++pos; return n; }
	if (t.kind == TokKind::Ident) { auto v = std::make_unique<VarNode>(t.text); ++pos; return v; }
	if (t.kind == TokKind::LParen) { ++pos; auto e = parse_expression(toks, pos); if (toks[pos].kind!=TokKind::RParen) throw std::runtime_error("missing )"); ++pos; return e; }
	throw std::runtime_error("unexpected token in primary");
}

static NodePtr parse_term(std::vector<Tok>& toks, size_t& pos) {
	auto node = parse_primary(toks,pos);
	while (toks[pos].kind==TokKind::Mul || toks[pos].kind==TokKind::Div) {
		char op = toks[pos].kind==TokKind::Mul? '*':'/'; ++pos;
		node = std::make_unique<BinaryNode>(op, std::move(node), parse_primary(toks,pos));
	}
	return node;
}

NodePtr parse_expression(std::vector<Tok>& toks, size_t& pos) {
	auto node = parse_term(toks,pos);
	while (toks[pos].kind==TokKind::Plus || toks[pos].kind==TokKind::Minus) {
		char op = toks[pos].kind==TokKind::Plus? '+':'-'; ++pos;
		node = std::make_unique<BinaryNode>(op, std::move(node), parse_term(toks,pos));
	}
	return node;
}

NodePtr parse_statement(std::vector<Tok>& toks, size_t& pos) {
	if (toks[pos].kind==TokKind::Ident && toks[pos+1].kind==TokKind::Assign) {
		std::string name = toks[pos].text; pos+=2;
		auto expr = parse_expression(toks,pos);
		return std::make_unique<AssignNode>(name, std::move(expr));
	}
	return parse_expression(toks,pos);
}

static double eval_node(Node* n, std::unordered_map<std::string,double>& sym) {
	if (auto nn = dynamic_cast<NumberNode*>(n)) return nn->value;
	if (auto vn = dynamic_cast<VarNode*>(n)) {
		auto it = sym.find(vn->name);
		if (it==sym.end()) return 0.0; return it->second;
	}
	if (auto bn = dynamic_cast<BinaryNode*>(n)) {
		double L = eval_node(bn->left.get(), sym);
		double R = eval_node(bn->right.get(), sym);
		switch (bn->op) { case '+': return L+R; case '-': return L-R; case '*': return L*R; case '/': return L/R; }
	}
	if (auto an = dynamic_cast<AssignNode*>(n)) {
		double v = eval_node(an->expr.get(), sym);
		sym[an->name] = v; return v;
	}
	throw std::runtime_error("unknown node in eval");
}

double run(NodePtr& root) {
	std::unordered_map<std::string,double> symbols;
	return eval_node(root.get(), symbols);
}

int repl() {
	std::string line;
	std::cout << "altc> ";
	while (std::getline(std::cin,line)) {
		if (line.empty()) { std::cout<<"altc> "; continue; }
		std::stringstream ss(line);
		auto toks = lex(ss);
		size_t pos = 0;
		try {
			auto root = parse_statement(toks,pos);
			std::unordered_map<std::string,double> symbols;
			double res = eval_node(root.get(), symbols);
			std::cout << "= " << res << "\n";
		} catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << "\n";
		}
		std::cout<<"altc> ";
	}
	return 0;
}

}
#include "../includes/Compiler.hpp"

int32_t insideFunction = 0;

int32_t priority(Node *node)
{
	if (node->name == "*" || node->name == "/")
		return 2;
	if (node->name == "+" || node->name == "-")
		return 1;
	return 0;
}

std::unique_ptr<Node> parseExpression(std::vector<Token> &tokens, SymbolTable &ST)
{
	State state = State::Start;

	std::stack<std::unique_ptr<Node>> operators;
	std::stack<std::unique_ptr<Node>> operands;

	for (const Token &t : tokens)
	{
		state = FSM[static_cast<int32_t>(state)][static_cast<int32_t>(t.type)];

		if (state == State::Error)
			throw std::runtime_error("unexpected token " + t.value);
		if (state == State::End)
			break;

		std::unique_ptr<Node> n = std::make_unique<Node>(t);

		if (t.type == NodeType::Num)
			operands.push(std::move(n));
		else if (t.type == NodeType::Var)
		{
			n->symAddr = ST.getAddress(n->name);
			operands.push(std::move(n));
		}
		else if (t.type == NodeType::Op || t.type == NodeType::Comp)
		{
			while (!operators.empty() &&
				   operators.top()->type != NodeType::OpBr &&
				   priority(operators.top().get()) >= priority(n.get()))
			{
				std::unique_ptr<Node> op = std::move(operators.top());
				operators.pop();

				if (operands.size() < 2)
					throw std::runtime_error("invalid expression");

				std::unique_ptr<Node> right = std::move(operands.top());
				operands.pop();
				std::unique_ptr<Node> left = std::move(operands.top());
				operands.pop();

				op->left = std::move(left);
				op->right = std::move(right);

				operands.push(std::move(op));
			}
			operators.push(std::move(n));
		}
		else if (t.type == NodeType::OpBr)
			operators.push(std::move(n));
		else if (t.type == NodeType::ClBr)
		{
			while (!operators.empty() && operators.top()->type != NodeType::OpBr)
			{
				std::unique_ptr<Node> op = std::move(operators.top());
				operators.pop();

				if (operands.size() < 2)
					throw std::runtime_error("invalid expression");

				std::unique_ptr<Node> right = std::move(operands.top());
				operands.pop();
				std::unique_ptr<Node> left = std::move(operands.top());
				operands.pop();

				op->left = std::move(left);
				op->right = std::move(right);

				operands.push(std::move(op));
			}

			if (operators.empty())
				throw std::runtime_error("Mismatched parentheses");

			operators.pop();
		}
	}

	if (state != State::End)
		throw std::runtime_error("unexpected end");

	while (!operators.empty())
	{
		std::unique_ptr<Node> op = std::move(operators.top());
		operators.pop();

		if (operands.size() < 2)
			throw std::runtime_error("invalid expression");

		std::unique_ptr<Node> right = std::move(operands.top());
		operands.pop();
		std::unique_ptr<Node> left = std::move(operands.top());
		operands.pop();

		op->left = std::move(left);
		op->right = std::move(right);

		operands.push(std::move(op));
	}

	if (operands.empty())
		throw std::runtime_error("Empty expression");
	return std::move(operands.top());
}

std::unique_ptr<Node> parseAssign(std::vector<Token> &tokens, SymbolTable &ST, int32_t &pos)
{
	if (tokens[pos].type != NodeType::Var)
		throw std::runtime_error("Expected variable name " + tokens[pos].value);

	if (!ST.isDeclared(tokens[pos].value))
		throw std::runtime_error("Variable " + tokens[pos].value + " is not declared");

	auto node = std::make_unique<Node>(NodeType::Assign);
	node->left = std::make_unique<Node>(tokens[pos]);
	node->left->symAddr = ST.getAddress(tokens[pos].value);

	pos++;

	if (tokens[pos].type != NodeType::Assign)
		throw std::runtime_error("Expected = " + tokens[pos].value);

	pos++;

	std::vector<Token> expr;
	while (tokens[pos].type != NodeType::Semi)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Expected ; " + tokens[pos].value);
		expr.push_back(tokens[pos++]);
	}
	expr.push_back(Token("", NodeType::EofEx));
	node->right = parseExpression(expr, ST);

	pos++;
	return node;
}

std::unique_ptr<Node> parseIf(std::vector<Token> &tokens, SymbolTable &ST, int32_t &pos)
{
	if (tokens[pos].type != NodeType::If)
		throw std::runtime_error("Expected if " + tokens[pos].value);

	pos++;
	std::vector<Token> cond;

	if (tokens[pos].type != NodeType::OpBr)
		throw std::runtime_error("Expected ( " + tokens[pos].value);

	pos++;

	while (tokens[pos].type != NodeType::ClBr)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Missing )");
		cond.push_back(tokens[pos++]);
	}
	cond.push_back(Token("", NodeType::EofEx));

	pos++;

	auto node = std::make_unique<IfNode>();
	node->condition = parseExpression(cond, ST);

	if (tokens[pos].type == NodeType::OpBody)
		node->trueBranch = parseBlock(tokens, ST, pos);
	else
		node->trueBranch = parseStatement(tokens, ST, pos);

	if (tokens[pos].type == NodeType::Else)
	{
		pos++;
		if (tokens[pos].type == NodeType::If)
			node->falseBranch = parseIf(tokens, ST, pos);
		else if (tokens[pos].type == NodeType::OpBody)
			node->falseBranch = parseBlock(tokens, ST, pos);
		else
			node->falseBranch = parseStatement(tokens, ST, pos);
	}
	return node;
}

std::unique_ptr<Node> parseWhile(std::vector<Token> &tokens, SymbolTable &ST, int32_t &pos)
{
	if (tokens[pos].type != NodeType::While)
		throw std::runtime_error("Expected while " + tokens[pos].value);

	pos++;
	std::vector<Token> cond;

	if (tokens[pos].type != NodeType::OpBr)
		throw std::runtime_error("Expected ( " + tokens[pos].value);

	pos++;

	while (tokens[pos].type != NodeType::ClBr)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Expected ) " + tokens[pos].value);
		cond.push_back(tokens[pos++]);
	}
	cond.push_back(Token("", NodeType::EofEx));

	pos++;

	auto node = std::make_unique<WhileNode>();
	node->condition = parseExpression(cond, ST);

	if (tokens[pos].type != NodeType::OpBody)
		throw std::runtime_error("Expected { " + tokens[pos].value);

	node->body = parseBlock(tokens, ST, pos);

	return node;
}

std::unique_ptr<Node> parseVarDecl(std::vector<Token> &tokens, SymbolTable &ST, int32_t &pos)
{
	if (tokens[pos].type != NodeType::Type)
		throw std::runtime_error("Expected type" + tokens[pos].value);

	pos++;
	if (tokens[pos].type != NodeType::Var)
		throw std::runtime_error("Expected variable name" + tokens[pos].value);

	ST.declareVariable(tokens[pos].value);

	std::string varName = tokens[pos].value;

	if (tokens[pos + 1].type == NodeType::Assign)
		return parseAssign(tokens, ST, pos);

	pos++;

	if (tokens[pos].type != NodeType::Semi)
		throw std::runtime_error("Expected ';' after declaration" + tokens[pos].value);

	pos++;

	return nullptr;
}

std::unique_ptr<Node> parseReturn(std::vector<Token> &tokens, SymbolTable &ST, int32_t &pos)
{
	if (tokens[pos].type != NodeType::Ret)
		throw std::runtime_error("Expected return");

	pos++;

	auto node = std::make_unique<ReturnNode>();

	if (tokens[pos].type == NodeType::Semi)
	{
		pos++;
		return node;
	}

	std::vector<Token> expr;

	while (tokens[pos].type != NodeType::Semi)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Missing ';' after return");

		expr.push_back(tokens[pos++]);
	}

	pos++;
	expr.push_back(Token("", NodeType::EofEx));

	node->expr = parseExpression(expr, ST);

	return node;
}

std::unique_ptr<Node> parseCall(std::vector<Token> &tokens, SymbolTable &ST, int32_t &pos)
{
	if (tokens[pos].type != NodeType::Var)
		throw std::runtime_error("Expected function name");

	auto node = std::make_unique<CallNode>();
	node->name = tokens[pos].value;

	pos++;

	if (tokens[pos].type != NodeType::OpBr)
		throw std::runtime_error("Expected (");

	pos++;

	while (tokens[pos].type != NodeType::ClBr)
	{
		std::vector<Token> expr;

		int32_t depth = 0;

		while (!(tokens[pos].type == NodeType::Comma && depth == 0) &&
			   !(tokens[pos].type == NodeType::ClBr && depth == 0))
		{
			if (tokens[pos].type == NodeType::OpBr)
				depth++;
			if (tokens[pos].type == NodeType::ClBr)
				depth--;

			expr.push_back(tokens[pos++]);
		}

		expr.push_back(Token("", NodeType::EofEx));

		node->args.push_back(parseExpression(expr, ST));

		if (tokens[pos].type == NodeType::Comma)
			pos++;
	}

	pos++;

	if (tokens[pos].type == NodeType::Semi)
		pos++;

	return node;
}

std::unique_ptr<Node> parseStatement(std::vector<Token> &tokens, SymbolTable &ST, int &pos)
{
	if (static_cast<size_t>(pos) >= tokens.size())
		throw std::runtime_error("Unexpected end of input");

	switch (tokens[pos].type)
	{
	case NodeType::If:
		return parseIf(tokens, ST, pos);
	case NodeType::Type:
		return parseVarDecl(tokens, ST, pos);
	case NodeType::Var:
	{
		if (static_cast<size_t>(pos + 1) < tokens.size() && tokens[pos + 1].type == NodeType::OpBr)
			return parseCall(tokens, ST, pos);

		if (static_cast<size_t>(pos + 1) < tokens.size() && tokens[pos + 1].type == NodeType::Assign)
			return parseAssign(tokens, ST, pos);
		break;
	}
	case NodeType::OpBody:
		return parseBlock(tokens, ST, pos);
	case NodeType::While:
		return parseWhile(tokens, ST, pos);
	case NodeType::Ret:
	{
		if (!insideFunction)
			throw std::runtime_error("return outside of function");
		return parseReturn(tokens, ST, pos);
	}
	default:
		break;
	}
	std::vector<Token> expr;
	while (static_cast<size_t>(pos) < tokens.size() && tokens[pos].type != NodeType::Semi)
		expr.push_back(tokens[pos++]);

	if (static_cast<size_t>(pos) >= tokens.size())
		throw std::runtime_error("Missing ';'");
	expr.push_back(Token("", NodeType::EofEx));
	pos++;
	return parseExpression(expr, ST);
}

std::unique_ptr<Node> parseBlock(std::vector<Token> &tokens, SymbolTable &ST, int &pos)
{
	auto block = std::make_unique<BlockNode>();

	if (tokens[pos].type != NodeType::OpBody)
		throw std::runtime_error("Expected { " + tokens[pos].value);

	pos++;

	ST.enterScope();

	while (tokens[pos].type != NodeType::ClBody)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Expected } " + tokens[pos].value);

		auto stmt = parseStatement(tokens, ST, pos);

		if (stmt)
			block->statements.push_back(std::move(stmt));
	}

	ST.exitScope();

	pos++;

	return block;
}

std::unique_ptr<Node> parseFunction(std::vector<Token> &tokens, SymbolTable &ST, int &pos)
{
	if (tokens[pos].type != NodeType::Func)
		throw std::runtime_error("Expected keyword \"Func\" " + tokens[pos].value);

	pos++;
	if (tokens[pos].type != NodeType::Type)
		throw std::runtime_error("Expected a type " + tokens[pos].value);

	auto func = std::make_unique<FuncNode>();
	if (tokens[pos].value == "int")
		func->retType = ReturnType::_int;
	else if (tokens[pos].value == "void")
		func->retType = ReturnType::_void;
	else
		throw std::runtime_error("Expected a type " + tokens[pos].value);

	pos++;

	if (tokens[pos].type != NodeType::Var)
		throw std::runtime_error("Expected function name " + tokens[pos].value);

	func->name = tokens[pos].value;

	pos++;

	if (tokens[pos].type != NodeType::OpBr)
		throw std::runtime_error("Expected ( " + tokens[pos].value);

	pos++;
	ST.enterScope();

	while (tokens[pos].type != NodeType::ClBr)
	{
		if (tokens[pos].type != NodeType::Type)
			throw std::runtime_error("Expected parameter type " + tokens[pos].value);

		std::string type = tokens[pos].value;
		pos++;

		if (tokens[pos].type != NodeType::Var)
			throw std::runtime_error("Expected parameter name " + tokens[pos].value);

		std::string name = tokens[pos].value;

		if (ST.isDeclared(name))
			throw std::runtime_error("Duplicate parameter: " + name);

		ST.declareVariable(name);

		pos++;

		func->params.push_back({type, name, ST.getAddress(name)});

		if (tokens[pos].type == NodeType::Comma)
			pos++;
		else if (tokens[pos].type != NodeType::ClBr)
			throw std::runtime_error(
				"Expected ',' or ')' " + tokens[pos].value);
	}
	pos++;
	if (tokens[pos].type != NodeType::OpBody)
		throw std::runtime_error("Expected { for function body " + tokens[pos].value);

	insideFunction = 1;
	func->body = std::unique_ptr<BlockNode>(static_cast<BlockNode *>(parseBlock(tokens, ST, pos).release()));
	insideFunction = 0;
	ST.exitScope();
	for (auto &stmt : func->body->statements)
	{
		if (stmt->type == NodeType::Ret)
		{
			func->returnNode = static_cast<ReturnNode *>(stmt.get());
			break;
		}
	}
	return func;
}

std::vector<std::unique_ptr<Node>> parser(std::vector<Token> &tokens, SymbolTable &ST, int32_t &pos)
{
	std::vector<std::unique_ptr<Node>> functions;

	std::unordered_set<std::string> functionNames;

	while (tokens[pos].type != NodeType::EofEx)
	{
		int32_t oldPos = pos;

		if (tokens[pos].type != NodeType::Func)
			throw std::runtime_error("Only functions allowed at global scope");

		auto func = parseFunction(tokens, ST, pos);

		FuncNode *fn = dynamic_cast<FuncNode *>(func.get());

		if (!fn)
			throw std::runtime_error("Internal parser error");

		if (functionNames.find(fn->name) != functionNames.end())
			throw std::runtime_error("Multiple definition of function: " + fn->name);

		if (fn->retType == ReturnType::_void && fn->returnNode != nullptr &&
			fn->returnNode->expr != nullptr)
			throw std::runtime_error("Void function cannot return a value");

		if (fn->retType == ReturnType::_int && fn->returnNode == nullptr)
			throw std::runtime_error("Function should return a value");

		functionNames.insert(fn->name);

		functions.push_back(std::move(func));

		if (pos == oldPos)
			throw std::runtime_error("Parser stuck (no progress)");
	}

	return functions;
}