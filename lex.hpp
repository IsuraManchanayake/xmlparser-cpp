#pragma once

#include "token.hpp"

inline bool is_white_space(char c) {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

struct Lexer {
  std::string source;
  TokenHolder tokens;

  Lexer(std::string source, size_t token_cap)
      : source(std::move(source)), tokens(token_cap) {}
  void scan() {
    std::string::const_iterator iter = source.begin();
    while (iter != source.cend()) {
      switch (*iter) {
      case '<': {
        ++iter;
        tokens.emplace_back(TokenKind::LESS);
        break;
      }
      case '/': {
        ++iter;
        if (tokens.back().kind == TokenKind::LESS) {
          tokens.pop_back();
          tokens.emplace_back(TokenKind::LESS_SLASH);
        } else {
          tokens.emplace_back(TokenKind::SLASH);
        }
        break;
      }
      case '>': {
        ++iter;
        if (tokens.back().kind == TokenKind::SLASH) {
          tokens.pop_back();
          tokens.emplace_back(TokenKind::GREAT_SLASH);
        } else {
          tokens.emplace_back(TokenKind::GREAT);
        }
        break;
      }
      default: {
        auto start = iter;
        bool ws = true;
        while (*iter != 0 && *iter != '<' && *iter != '>' && *iter != '/') {
          ws &= is_white_space(*iter);
          ++iter;
        }
        if (ws) {
          tokens.emplace_back(TokenKind::WHITESPACE);
        } else {
          tokens.emplace_back(TokenKind::ELEMENT, start, iter);
        }
      }
      }
    }
    tokens.emplace_back(TokenKind::ENDOFFILE);
  }
};