#include "../includes/Compiler.hpp"
#include <cctype>

namespace compiler {

Tok make_number(const std::string& s) {
    Tok t;
    t.kind = TokKind::Num;
    t.value = std::stod(s);
    t.text = s;
    return t;
}

std::vector<Tok> lex(std::stringstream& in) {
    std::vector<Tok> out;
    char c;
    while (in.get(c)) {
        if (std::isspace((unsigned char)c)) continue;
        if (std::isdigit((unsigned char)c) || c == '.') {
            std::string num(1, c);
            while (in.peek() != EOF && (std::isdigit(in.peek()) || in.peek() == '.')) {
                num.push_back((char)in.get());
            }
            out.push_back(make_number(num));
            continue;
        }
        if (std::isalpha((unsigned char)c) || c == '_') {
            std::string id(1, c);
            while (std::isalnum(in.peek()) || in.peek() == '_') id.push_back((char)in.get());
            Tok t; t.kind = TokKind::Ident; t.text = id; out.push_back(t); continue;
        }
        switch (c) {
            case '+': out.push_back({TokKind::Plus,0.0,"+"}); break;
            case '-': out.push_back({TokKind::Minus,0.0,"-"}); break;
            case '*': out.push_back({TokKind::Mul,0.0,"*"}); break;
            case '/': out.push_back({TokKind::Div,0.0,"/"}); break;
            case '(' : out.push_back({TokKind::LParen,0.0,"("}); break;
            case ')' : out.push_back({TokKind::RParen,0.0,")"}); break;
            case '=' : out.push_back({TokKind::Assign,0.0,"="}); break;
            default: out.push_back({TokKind::Unknown,0.0,std::string(1,c)}); break;
        }
    }
    out.push_back({TokKind::End,0.0,""});
    return out;
}

}
	std::string s;
	while (line.peek() != EOF && std::isdigit(line.peek()))
		s += line.get();
	words.push_back(s);
}

void readVar(std::stringstream& line, std::vector<std::string>& words)
{
	std::string s;
	while (line.peek() != EOF && std::isalnum(line.peek()))
		s += line.get();
	words.push_back(s);
}

bool isoperator(const char c)
{
	return c == '+' || c == '-' || c == '*' || c == '/';
}

std::vector<std::string> lexer(std::stringstream& line)
{
	char ch;
	std::vector<std::string> words;
	std::string s;
	while (line.get(ch))
	{
		if (std::isspace(ch))
			continue;
		else if (std::isdigit(ch))
		{
			line.putback(ch);
			readNum(line, words);
		}
		else if (std::isalpha(ch))
		{
			line.putback(ch);
			readVar(line, words);
		}
		else if (isoperator(ch) || ch == '(' || ch == ')' || ch == ';' || ch == '{' || ch == '}' || ch == ',')
			words.push_back(std::string(1, ch));
		else if (ch == '>' || ch == '<' || ch == '=')
		{
			words.push_back(std::string(1, ch));
			if (line.peek() == '=')
			{
				line.get(ch);
				words.back() += ch;
			}
		}
		else if (ch == '!' && line.peek() == '=')
		{
			words.push_back(std::string(1, ch));
			line.get(ch);
			words.back() += ch;
		}
		else
			throw std::runtime_error("unexpected input");
	}
	return words;
}