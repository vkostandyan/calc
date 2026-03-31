#include "AST.hpp"
#include "Evaluator.hpp"

namespace calc {

NumberNode::NumberNode(double value) : value_(value) {}

double NumberNode::evaluate(SymbolTable &symtab) const {
    return value_;
}

VariableNode::VariableNode(const std::string &name) : name_(name) {}

double VariableNode::evaluate(SymbolTable &symtab) const {
    if (symtab.find(name_) == symtab.end()) {
        throw EvaluationError("Undefined variable: " + name_);
    }
    return symtab[name_];
}

BinaryOpNode::BinaryOpNode(TokenType op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
    : op_(op), left_(std::move(left)), right_(std::move(right)) {}

double BinaryOpNode::evaluate(SymbolTable &symtab) const {
    double lhs = left_->evaluate(symtab);
    double rhs = right_->evaluate(symtab);

    switch (op_) {
        case TokenType::Plus:  return lhs + rhs;
        case TokenType::Minus: return lhs - rhs;
        case TokenType::Mul:   return lhs * rhs;
        case TokenType::Div:
            if (rhs == 0) throw EvaluationError("Division by zero");
            return lhs / rhs;
        default:
            throw EvaluationError("Invalid binary operator");
    }
}

AssignmentNode::AssignmentNode(const std::string &name, std::unique_ptr<ASTNode> value)
    : name_(name), value_(std::move(value)) {}

double AssignmentNode::evaluate(SymbolTable &symtab) const {
    double result = value_->evaluate(symtab);
    symtab[name_] = result;
    return result;
}

}
