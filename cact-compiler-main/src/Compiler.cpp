#include <iostream>
#include "../include/util/IList.h"
#include "frontend/Lexer.h"
#include "frontend/Parser.h"
#include "../include/frontend/Visitor.h"
#include "../include/backend/RiscVCodeGen.h"
#include <fstream>
#include "sstream"
#include "iostream"
#include "Init.h"
#include "FrontendInit.h"
#include <chrono>
#include <string>
#include <cstring>
#include <cstdlib>

/* Argument Parse begin */

std::string input_file, ir_file, asm_file;
bool emit_ir = false;
bool emit_asm = false;

void parse_args(int argc, char *argv[]) {
    // Usage: ./compiler [-emit-ir <ir_file>] [-emit-asm <asm_file>] <input_file>
    
    for (int i = 1; i < argc; i++) {
        if (i + 1 < argc && strcmp(argv[i], "-emit-ir") == 0) {
            ir_file.assign(argv[i + 1]);
            emit_ir = true;
            i += 1;
            continue;
        }
        if (i + 1 < argc && strcmp(argv[i], "-emit-asm") == 0) {
            asm_file.assign(argv[i + 1]);
            emit_asm = true;
            i += 1;
            continue;
        }
        input_file.assign(argv[i]);
    }
    
    if (input_file.empty()) {
        std::cerr << "error: need input file." << std::endl;
        exit(1);
    }
    
    if (!emit_ir && !emit_asm) {
        std::cerr << "error: need output option (-emit-ir or -emit-asm)." << std::endl;
        exit(1);
    }
}

/* Argument Parse end */

int main(int argc, char *argv[]) {
    parse_args(argc, argv);

    Manager::external = false;
    FileDealer::inputDealer(input_file.c_str());
    Lexer lexer = Lexer();
    lexer.lex();
    Parser parser = Parser(lexer.tokenList);
    std::cerr << "Parser & Visitor begin" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    AST::Ast *ast = parser.parseAst();
    Visitor visitor = Visitor();
    visitor.visitAst(ast);
    auto end = std::chrono::high_resolution_clock::now();
    std::cerr << "Parser & Visitor end, Use Time: " << std::chrono::duration<double>(end - start).count() << "s"
              << std::endl;

    // Output LLVM IR if requested
    if (emit_ir) {
        Manager::MANAGER->outputLLVM(ir_file);
        std::cerr << "LLVM IR generated: " << ir_file << std::endl;
    }
    
    // Output RISC-V assembly if requested
    if (emit_asm) {
        std::cerr << "Generating RISC-V assembly..." << std::endl;
        auto codeGenStart = std::chrono::high_resolution_clock::now();
        
        RiscV::RiscVCodeGen codeGen(Manager::MANAGER);
        codeGen.generateCode();
        codeGen.outputAssembly(asm_file);
        
        auto codeGenEnd = std::chrono::high_resolution_clock::now();
        std::cerr << "RISC-V assembly generated: " << asm_file << std::endl;
        std::cerr << "Code generation time: " << std::chrono::duration<double>(codeGenEnd - codeGenStart).count() << "s" << std::endl;
    }
    
    std::cerr << "CACT compilation finished successfully." << std::endl;
    return 0;
}
