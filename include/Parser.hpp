#pragma once
#include "AST.hpp"
#include "Lexer.hpp"

namespace calc {

class Parser {
public:
    explicit Parser(Lexer lexer);
    std::unique_ptr<ASTNode> parse();

private:
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();

    Lexer lexer_;
    Token currentToken_;
    void eat(TokenType type);
};

}
