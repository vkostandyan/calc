#include "../includes/Token.hpp"

bool isNum(const std::string &word)
{
	for (size_t i = 0; i < word.length(); i++)
	{
		if (!std::isdigit(word[i]))
			return false;
	}
	return true;
}

bool isVar(const std::string &word)
{
	for (size_t i = 0; i < word.length(); i++)
	{
		if (!std::isalnum(word[i]))
			return false;
	}
	if (!std::isalpha(word[0]))
		return false;
	return true;
}

bool isComp(const std::string& word)
{
	if (word == "<" || word == ">" || word == "<=" || word == ">=" || word == "!=" || word == "==")
		return true;
	return false;
}

std::vector<Token> tokenizer(const std::vector<std::string> &words)
{
	std::vector<Token> tokens;
	for (size_t i = 0; i < words.size(); i++)
	{
		if (words[i].empty())
			continue;
		else if (words[i] == "int" || words[i] == "void")
			tokens.push_back(Token(words[i], NodeType::Type));
		else if (words[i] == "return")
			tokens.push_back(Token(words[i], NodeType::Ret));
		else if (words[i] == "Func")
			tokens.push_back(Token(words[i], NodeType::Func));
		else if (words[i] == "if")
			tokens.push_back(Token(words[i], NodeType::If));
		else if (words[i] == "else")
			tokens.push_back(Token(words[i], NodeType::Else));
		else if (words[i] == "while")
			tokens.push_back(Token(words[i], NodeType::While));
		else if (isNum(words[i]))
			tokens.push_back(Token(words[i], NodeType::Num));
		else if (isVar(words[i]))
			tokens.push_back(Token(words[i], NodeType::Var));
		else if (words[i] == "(")
			tokens.push_back(Token(words[i], NodeType::OpBr));
		else if (words[i] == ")")
			tokens.push_back(Token(words[i], NodeType::ClBr));
		else if (words[i] == "+" || words[i] == "-" || words[i] == "*" || words[i] == "/")
			tokens.push_back(Token(words[i], NodeType::Op));
		else if (isComp(words[i]))
			tokens.push_back(Token(words[i], NodeType::Comp));
		else if (words[i] == "=")
			tokens.push_back(Token(words[i], NodeType::Assign));
		else if (words[i] == "!")
			tokens.push_back(Token(words[i], NodeType::Not));
		else if (words[i] == ";")
			tokens.push_back(Token(words[i], NodeType::Semi));
		else if (words[i] == "{")
			tokens.push_back(Token(words[i], NodeType::OpBody));
		else if (words[i] == "}")
			tokens.push_back(Token(words[i], NodeType::ClBody));
		else if (words[i] == ",")
			tokens.push_back(Token(words[i], NodeType::Comma));
		else
			throw std::runtime_error("unexpected token " + words[i]);
	}
	tokens.push_back(Token("", NodeType::EofEx));
	return tokens;
}