#pragma once
#include "Token.hpp"
#include <string>

namespace calc {

class Lexer {
public:
    explicit Lexer(const std::string &input);
    Token nextToken();
    Token peekToken();

private:
    void consumeWhitespace();
    Token consumeNumber();
    Token consumeIdentifier();

    std::string input_;
    size_t pos_;
    Token lookahead_;
    bool hasLookahead_;
};

}
