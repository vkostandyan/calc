#pragma once
#include "AST.hpp"
#include <stdexcept>
#include <map>

namespace calc {

class EvaluationError : public std::runtime_error {
public:
    explicit EvaluationError(const std::string &message);
};

class Evaluator {
public:
    Evaluator() = default;
    double evaluate(const ASTNode &ast);
    const SymbolTable &getSymbols() const { return symtab_; }

private:
    SymbolTable symtab_;
};

}