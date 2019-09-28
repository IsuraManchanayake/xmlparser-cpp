#pragma once

#include "token.hpp"

inline bool is_white_space(char c) {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

#define _next_char() (++iter, ++col)

struct Lexer {
  std::string source;
  TokenHolder tokens;

  Lexer(std::string source, size_t token_cap)
      : source(std::move(source)), tokens(token_cap) {}
  void scan() {
    size_t line = 0;
    size_t col = 0;
    std::string::const_iterator iter = source.begin();
    while (iter != source.cend()) {
      switch (*iter) {
      case '<': {
        _next_char();
        tokens.emplace_back(TokenKind::LESS, line, col);
        break;
      }
      case '/': {
        _next_char();
        if (tokens.back().kind == TokenKind::LESS) {
          tokens.pop_back();
          tokens.emplace_back(TokenKind::LESS_SLASH, line, col);
        } else {
          tokens.emplace_back(TokenKind::SLASH, line, col);
        }
        break;
      }
      case '>': {
        _next_char();
        if (tokens.back().kind == TokenKind::SLASH) {
          tokens.pop_back();
          tokens.emplace_back(TokenKind::GREAT_SLASH, line, col);
        } else {
          tokens.emplace_back(TokenKind::GREAT, line, col);
        }
        break;
      }
      default: {
        auto start = iter;
        bool ws = true;
        while (*iter != 0 && *iter != '<' && *iter != '>' && *iter != '/') {
          ws &= is_white_space(*iter);
          if (*iter == '\n') {
            ++line;
            col = 0;
          }
          _next_char();
        }
        if (ws) {
          tokens.emplace_back(TokenKind::WHITESPACE, line, col);
        } else {
          auto end = iter;
          while (is_white_space(*start))
            ++start;
          while (is_white_space(*(end - 1)))
            --end;
          tokens.emplace_back(TokenKind::ELEMENT, start, end, line, col);
        }
      }
      }
    }
    tokens.emplace_back(TokenKind::ENDOFFILE, line, col);
  }
};

#undef _next_char
