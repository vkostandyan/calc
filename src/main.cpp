#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "Evaluator.hpp"

#include <iostream>
#include <string>

int main() {
    calc::Evaluator evaluator;
    std::string line;
    
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        try {
            calc::Lexer lexer(line);
            calc::Parser parser(std::move(lexer));
            auto ast = parser.parse();
            double result = evaluator.evaluate(*ast);
            std::cout << "= " << result << "\n";
            
            const auto &symbols = evaluator.getSymbols();
            if (!symbols.empty()) {
                std::cout << "Variables: ";
                for (const auto &pair : symbols) {
                    std::cout << pair.first << "=" << pair.second << " ";
                }
                std::cout << "\n";
            }
        } catch (const calc::EvaluationError &e) {
            std::cerr << "Evaluation error: " << e.what() << "\n";
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}
