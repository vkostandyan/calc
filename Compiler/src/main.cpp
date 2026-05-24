#include "../includes/Compiler.hpp"
#include <iostream>
#include <sstream>

int main_compiler(int argc, char** argv) {
    if (argc > 1) {
        std::stringstream ss;
        for (int i=1;i<argc;++i) { ss<<argv[i]; if (i+1<argc) ss<<" "; }
        auto toks = compiler::lex(ss);
        size_t pos = 0;
        try {
            auto root = compiler::parse_statement(toks,pos);
            double r = compiler::run(root);
            std::cout << r << std::endl;
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 2;
        }
    }
    return compiler::repl();
}

int main(int argc, char** argv) { return main_compiler(argc, argv); }
