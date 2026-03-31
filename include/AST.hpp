#pragma once
#include "Token.hpp"
#include <memory>
#include <map>

namespace calc {

using SymbolTable = std::map<std::string, double>;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual double evaluate(SymbolTable &symtab) const = 0;
};

class NumberNode : public ASTNode {
public:
    explicit NumberNode(double value);
    double evaluate(SymbolTable &symtab) const override;

private:
    double value_;
};

class VariableNode : public ASTNode {
public:
    explicit VariableNode(const std::string &name);
    double evaluate(SymbolTable &symtab) const override;
    const std::string &getName() const { return name_; }

private:
    std::string name_;
};

class BinaryOpNode : public ASTNode {
public:
    BinaryOpNode(TokenType op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    double evaluate(SymbolTable &symtab) const override;

private:
    TokenType op_;
    std::unique_ptr<ASTNode> left_;
    std::unique_ptr<ASTNode> right_;
};

class AssignmentNode : public ASTNode {
public:
    AssignmentNode(const std::string &name, std::unique_ptr<ASTNode> value);
    double evaluate(SymbolTable &symtab) const override;

private:
    std::string name_;
    std::unique_ptr<ASTNode> value_;
};

}
