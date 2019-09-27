#pragma once

#include <iostream>
#include <fstream>

#include "lex.hpp"
#include "parse.hpp"

void test(const std::string &filename) {
  std::cout << "testing file: " << filename << '\n';
  std::ifstream ifs(filename);
  std::string source(std::istreambuf_iterator<char>{ifs}, {});
  Lexer lexer(source, 200);
  lexer.scan();
  Parser parser(filename, lexer.tokens.container);
  parser.buildSyntaxTree();

  for (const auto token : lexer.tokens.container) {
    std::cout << token << '\n';
  }
}