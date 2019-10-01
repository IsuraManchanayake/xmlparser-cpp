#pragma once

#include "exception.hpp"
#include "token.hpp"

inline bool is_white_space(char c) {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

char lut[256] = {0};
bool inited = false;
void initLut() {
  if (inited)
    return;
  lut['n'] = '\n';
  lut['r'] = '\r';
  lut['t'] = '\t';
  lut['\\'] = '\\';
  lut['\''] = '\'';
  inited = true;
}

#define _next_char() (++iter, ++col)
#define _reset_line()                                                          \
  do {                                                                         \
    ++line;                                                                    \
    col = 0;                                                                   \
  } while (0)

struct Lexer {
  const std::string filename;
  const std::string source;
  TokenHolder tokens;

  Lexer(std::string filename, std::string source, size_t token_cap)
      : filename(std::move(filename)), source(std::move(source)),
        tokens(token_cap) {}
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
          if (*iter == '\n')
            _reset_line();
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

struct TagLexer {
  const std::string filename;
  const std::string source;
  size_t line;
  size_t col;
  TokenHolder tokens;

  TagLexer(std::string filename, std::string source, size_t line, size_t col)
      : filename(std::move(filename)), source(std::move(source)), line(line),
        col(col) {
    initLut();
  }
  void scan() {
    std::string::const_iterator iter = source.begin();
    while (iter != source.cend()) {
      switch (*iter) {
      case '=': {
        _next_char();
        tokens.emplace_back(TokenKind::EQUALS, line, col);
        break;
      }
      case '\"': {
        _next_char();
        std::ostringstream ss;
        while (*iter != '\"') {
          if (*iter == '\\') {
            _next_char();
            char escape = lut[*iter];
            if (escape) {
              ss << escape;
            } else {
              throw UnknownEscapeCharacterError(filename, line, *iter);
            }
          } else if (is_white_space(*iter)) {
            ss << ' ';
            if (*iter == '\n')
              _reset_line();
          } else {
            ss << *iter;
          }
          _next_char();
        }
        tokens.emplace_back(TokenKind::DOUBLE_QUOTED_STRING, ss.str(), line, col);
        _next_char();
        break;
      }
      case ' ':
      case '\n':
      case '\r':
      case '\t': {
        _next_char();
        break;
      }
      default: {
        auto start = iter;
        while (*iter != 0 && *iter != '=' && *iter != '\"' &&
               !is_white_space(*iter)) {
          _next_char();
        }
        tokens.emplace_back(TokenKind::NAME, start, iter, line, col);
      }
      }
    }
    tokens.emplace_back(TokenKind::ENDOFFILE, line, col);
  }
};

#undef _next_char
#undef _reset_line
