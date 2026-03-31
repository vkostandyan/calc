#include "Lexer.hpp"
#include <cctype>

namespace calc {

Lexer::Lexer(const std::string &input)
    : input_(input), pos_(0), hasLookahead_(false) {}

void Lexer::consumeWhitespace() {
    while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_]))) {
        ++pos_;
    }
}

Token Lexer::consumeNumber() {
    size_t start = pos_;
    bool hasDot = false;
    while (pos_ < input_.size()) {
        char c = input_[pos_];
        if (c == '.') {
            if (hasDot) break;
            hasDot = true;
            ++pos_;
            continue;
        }
        if (!std::isdigit(static_cast<unsigned char>(c))) break;
        ++pos_;
    }
    double value = std::stod(input_.substr(start, pos_ - start));
    return Token(TokenType::Number, value, input_.substr(start, pos_ - start));
}

Token Lexer::consumeIdentifier() {
    size_t start = pos_;
    while (pos_ < input_.size() && (std::isalnum(static_cast<unsigned char>(input_[pos_])) || input_[pos_] == '_')) {
        ++pos_;
    }
    std::string name = input_.substr(start, pos_ - start);
    return Token(TokenType::Identifier, 0, name);
}

Token Lexer::nextToken() {
    if (hasLookahead_) {
        hasLookahead_ = false;
        return lookahead_;
    }

    consumeWhitespace();
    if (pos_ >= input_.size()) return Token(TokenType::End, 0, "");
    char current = input_[pos_];
    
    if (std::isdigit(static_cast<unsigned char>(current)) || current == '.') {
        return consumeNumber();
    }

    if (std::isalpha(static_cast<unsigned char>(current)) || current == '_') {
        return consumeIdentifier();
    }

    ++pos_;
    switch (current) {
        case '+': return Token(TokenType::Plus, 0, "+");
        case '-': return Token(TokenType::Minus, 0, "-");
        case '*': return Token(TokenType::Mul, 0, "*");
        case '/': return Token(TokenType::Div, 0, "/");
        case '(' : return Token(TokenType::LParen, 0, "(");
        case ')' : return Token(TokenType::RParen, 0, ")");
        case '=': return Token(TokenType::Equal, 0, "=");
        default:  return Token(TokenType::Invalid, 0, std::string(1, current));
    }
}

Token Lexer::peekToken() {
    if (!hasLookahead_) {
        lookahead_ = nextToken();
        hasLookahead_ = true;
    }
    return lookahead_;
}

}
