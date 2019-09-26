#pragma once

#include "lex.hpp"

struct Parser {
  const std::vector<Token>& tokens;

  Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
  void buildSyntaxTree() {
  }
};