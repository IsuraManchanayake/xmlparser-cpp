#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

enum class TokenKind {
  TAG_START,
  TAG,
  TAG_END,
  SLASH,
  ELEMENT,
  WHITESPACE,
};

struct Token {
  std::string str;
  TokenKind kind;
  Token(TokenKind kind) : kind(kind) {}
  Token(TokenKind kind, std::string str) : kind(kind), str(std::move(str)) {}
  Token(TokenKind kind, std::string::const_iterator start, std::string::const_iterator end)
      : kind(kind), str(start, end) {}
};

#define TOKEN_OUT(tok_name)                                                    \
  case TokenKind::tok_name:                                                    \
    os << #tok_name;                                                           \
    break;

std::ostream &operator<<(std::ostream &os, const Token &token) {
  os << "{Token kind:[";
  switch (token.kind) {
    TOKEN_OUT(TAG_START);
    TOKEN_OUT(TAG);
    TOKEN_OUT(TAG_END);
    TOKEN_OUT(SLASH);
    TOKEN_OUT(ELEMENT);
    TOKEN_OUT(WHITESPACE);
  }
  return os << "] str:[" << token.str << "]}";
}

#undef TOKEN_OUT

struct TokenHolder {
  static constexpr size_t max_size = 100;
  std::vector<Token> container;

  TokenHolder() {}
  template <typename... Args> void emplace_back(Args &&... args) {
    if (container.size() < max_size) {
      container.emplace_back(std::forward<Args>(args)...);
    } else {
      assert(0);
    }
  }
  const Token &back() const { return container.back(); }
};

struct Lexer {
  std::string source;
  TokenHolder tokens;

  Lexer(std::string source) : source(std::move(source)) {}
  void scan() {
    std::string::const_iterator iter = source.begin();
    while (iter != source.cend()) {
      switch (*iter) {
      case '<': {
        tokens.emplace_back(TokenKind::TAG_START);
        auto start = ++iter;
        while (*iter != '>') {
          ++iter;
        }
        if (*start == '/') {
          tokens.emplace_back(TokenKind::SLASH);
          ++start;
        }
        tokens.emplace_back(TokenKind::TAG, start, iter);
        break;
      }
      case '>': {
        tokens.emplace_back(TokenKind::TAG_END);
        ++iter;
        break;
      }
      case ' ':
      case '\n':
      case '\t':
      case '\r':
        // if (tokens.back().kind != TokenKind::WHITESPACE) {
        //   tokens.emplace_back(TokenKind::WHITESPACE);
        // }
        ++iter;
        break;
      default: {
        auto start = iter;
        while (*iter != 0 && *iter != '<') {
          ++iter;
        }
        tokens.emplace_back(TokenKind::ELEMENT, start, iter);
      }
      }
    }
  }
};