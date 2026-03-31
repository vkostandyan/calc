#pragma once
#include <string>

namespace calc {

enum class TokenType {
    Number,
    Identifier,
    Plus,
    Minus,
    Mul,
    Div,
    LParen,
    RParen,
    Equal,
    End,
    Invalid
};

struct Token {
    TokenType type;
    double number;
    std::string text;

    Token(TokenType type = TokenType::Invalid, double number = 0, std::string text = "")
        : type(type), number(number), text(std::move(text)) {}
};

}
