#pragma once

#include <cstddef>
#include <exception>
#include <sstream>
#include <string>

#include "token.hpp"

struct SyntaxError : public std::exception {
  const std::string file_name;
  const size_t line;

  SyntaxError(std::string file_name, size_t line)
      : file_name(file_name), line(line) {}
  virtual ~SyntaxError() {}
  virtual const char *what() const throw() = 0;
  std::string errorHeader() const {
    std::stringstream ss;
    ss << "Error:SyntaxError[" << file_name << ':' << line << ']';
    return ss.str();
  }
};

struct TokenNotFoundError : public SyntaxError {
  const TokenKind expected;
  const TokenKind found;

  TokenNotFoundError(std::string file_name, size_t line, TokenKind expected,
                     TokenKind found)
      : SyntaxError(file_name, line), expected(expected), found(found) {}

  virtual const char *what() const throw() {
    std::stringstream ss;
    ss << errorHeader() << ":Expected token {" << expected << "} But found {"
       << found << '}';
    return ss.str().c_str();
  }
};

struct ReachedEOFError : public SyntaxError {
  const size_t remainingTagsInStack;
  ReachedEOFError(std::string file_name, size_t line,
                  size_t remainingTagsInStack)
      : SyntaxError(file_name, line),
        remainingTagsInStack(remainingTagsInStack) {}

  virtual const char *what() const throw() {
    std::stringstream ss;
    ss << errorHeader() << ":Reached EOF while having " << remainingTagsInStack
       << " tags remaining in the stack";
    return ss.str().c_str();
  }
};

struct BadTagError : public SyntaxError {
  const std::string expectedTag;
  BadTagError(std::string file_name, size_t line, std::string expectedTag)
      : SyntaxError(file_name, line), expectedTag(std::move(expectedTag)) {}

  virtual const char *what() const throw() {
    std::stringstream ss;
    ss << errorHeader() << ":Expected tag {" << expectedTag << "} end";
    return ss.str().c_str();
  }
};
