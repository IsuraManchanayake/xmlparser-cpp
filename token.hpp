#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

enum class TokenKind {
  LESS,
  GREAT,
  LESS_SLASH,
  GREAT_SLASH,
  SLASH,
  TAG,
  ELEMENT,
  WHITESPACE,
  ENDOFFILE,
  // tag tokens
  NAME,
  EQUALS,
  DOUBLE_QUOTED_STRING,
};

struct Token {
  const TokenKind kind;
  const std::string str;
  const size_t line;
  const size_t col;
  Token(TokenKind kind, size_t line, size_t col)
      : kind(kind), line(line), col(col) {}
  Token(TokenKind kind, std::string str, size_t line, size_t col)
      : kind(kind), str(std::move(str)), line(line), col(col) {}
  Token(TokenKind kind, std::string::const_iterator start,
        std::string::const_iterator end, size_t line, size_t col)
      : kind(kind), str(start, end), line(line), col(col) {}
};

#define TOKEN_OUT(tok_name)                                                    \
  case TokenKind::tok_name:                                                    \
    os << #tok_name;                                                           \
    break;

std::ostream &operator<<(std::ostream &os, const TokenKind &tokenKind) {
  switch (tokenKind) {
    TOKEN_OUT(LESS);
    TOKEN_OUT(GREAT);
    TOKEN_OUT(LESS_SLASH);
    TOKEN_OUT(GREAT_SLASH);
    TOKEN_OUT(SLASH);
    TOKEN_OUT(TAG);
    TOKEN_OUT(ELEMENT);
    TOKEN_OUT(WHITESPACE);
    TOKEN_OUT(ENDOFFILE);
    TOKEN_OUT(NAME);
    TOKEN_OUT(EQUALS);
    TOKEN_OUT(DOUBLE_QUOTED_STRING);
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
  return os << "{Token(" << token.line << ':' << token.col << ") kind:["
            << token.kind << "] str:[" << token.str << "]}";
}

#undef TOKEN_OUT

struct TokenHolder {
  const size_t max_size;
  std::vector<Token> container;

  TokenHolder() : max_size(std::numeric_limits<size_t>::max()) {}
  TokenHolder(size_t max_size) : max_size(max_size) {}
  template <typename... Args> void emplace_back(Args &&... args) {
    if (container.size() < max_size) {
      container.emplace_back(std::forward<Args>(args)...);
    } else {
      std::cout << "Maximum size reached(" << max_size << ')' << std::endl;
      assert(0);
    }
  }
  const Token &back() const { return container.back(); }
  void pop_back() { return container.pop_back(); }
};