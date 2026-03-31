#include "Evaluator.hpp"

namespace calc {

EvaluationError::EvaluationError(const std::string &message)
    : std::runtime_error(message) {}

double Evaluator::evaluate(const ASTNode &ast) {
    return ast.evaluate(symtab_);
}

}