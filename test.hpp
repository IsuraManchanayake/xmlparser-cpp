#pragma once

#include <fstream>
#include <iostream>

#include "lex.hpp"
#include "parse.hpp"

void test(const std::string &filename) {
  std::cout << "testing file: " << filename << '\n';
  std::ifstream ifs(filename);
  std::string source(std::istreambuf_iterator<char>{ifs}, {});
  Lexer lexer(filename, source, 200);
  lexer.scan();
  for (size_t i = 0; i < lexer.tokens.container.size(); i++) {
    std::cout << i << ' ' << lexer.tokens.container[i] << '\n';
  }
  XMLParser parser(filename, lexer.tokens.container);
  XML *xml = parser.buildSyntaxTree();
  if (xml) {
    std::cout << xml << std::endl;
  }
  delete xml;
}