#pragma once

#include <fstream>
#include <iostream>

#include "lex.hpp"
#include "parse.hpp"

void test(const std::string &filename) {
  std::cout << "testing file: " << filename << '\n';
  std::ifstream ifs(filename);
  std::string source(std::istreambuf_iterator<char>{ifs}, {});
  Lexer lexer(source, 200);
  lexer.scan();
  for (size_t i = 0; i < lexer.tokens.container.size(); i++) {
    std::cout << i << ' ' << lexer.tokens.container[i] << '\n';
  }
  Parser parser(filename, lexer.tokens.container);
  parser.buildSyntaxTree();
}