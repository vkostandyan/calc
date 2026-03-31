#include "Parser.hpp"
#include <stdexcept>

namespace calc {

Parser::Parser(Lexer lexer)
    : lexer_(std::move(lexer)), currentToken_(lexer_.nextToken()) {}

void Parser::eat(TokenType type) {
    if (currentToken_.type == type) {
        currentToken_ = lexer_.nextToken();
    } else {
        throw std::runtime_error("Parser error: unexpected token " + currentToken_.text);
    }
}

std::unique_ptr<ASTNode> Parser::parse() {
    auto node = parseAssignment();
    if (currentToken_.type != TokenType::End) {
        throw std::runtime_error("Parser error: trailing input");
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    auto node = parseExpression();
    
    if (currentToken_.type == TokenType::Equal) {
        auto varNode = dynamic_cast<VariableNode*>(node.get());
        if (varNode) {
            std::string varName = varNode->getName();
            eat(TokenType::Equal);
            auto right = parseExpression();
            return std::make_unique<AssignmentNode>(varName, std::move(right));
        } else {
            throw std::runtime_error("Parser error: left side of assignment must be a variable");
        }
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto node = parseTerm();
    while (currentToken_.type == TokenType::Plus || currentToken_.type == TokenType::Minus) {
        TokenType op = currentToken_.type;
        eat(op);
        auto right = parseTerm();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto node = parseFactor();
    while (currentToken_.type == TokenType::Mul || currentToken_.type == TokenType::Div) {
        TokenType op = currentToken_.type;
        eat(op);
        auto right = parseFactor();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    if (currentToken_.type == TokenType::Number) {
        double value = currentToken_.number;
        eat(TokenType::Number);
        return std::make_unique<NumberNode>(value);
    }

    if (currentToken_.type == TokenType::Identifier) {
        std::string name = currentToken_.text;
        eat(TokenType::Identifier);
        return std::make_unique<VariableNode>(name);
    }

    if (currentToken_.type == TokenType::LParen) {
        eat(TokenType::LParen);
        auto node = parseExpression();
        eat(TokenType::RParen);
        return node;
    }

    if (currentToken_.type == TokenType::Minus) {
        eat(TokenType::Minus);
        auto factor = parseFactor();
        auto zero = std::make_unique<NumberNode>(0.0);
        return std::make_unique<BinaryOpNode>(TokenType::Minus, std::move(zero), std::move(factor));
    }

    throw std::runtime_error("Parser error: expected number, identifier, or '('");
}

}
