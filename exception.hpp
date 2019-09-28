#pragma once

#include <cstddef>
#include <exception>
#include <sstream>
#include <string>

#include "token.hpp"

struct SyntaxError : public std::exception {
  const std::string file_name;
  const size_t line;
  mutable std::string message;

  SyntaxError(std::string file_name, size_t line)
      : file_name(file_name), line(line), message() {}
  virtual ~SyntaxError() throw() {}
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

  const char *what() const throw() {
    std::stringstream ss;
    ss << errorHeader() << ":Expected token {" << expected << "} But found {"
       << found << '}';
    message = ss.str();
    return message.c_str();
  }
};

struct ReachedEOFError : public SyntaxError {
  const size_t remainingTagsInStack;
  ReachedEOFError(std::string file_name, size_t line,
                  size_t remainingTagsInStack)
      : SyntaxError(file_name, line),
        remainingTagsInStack(remainingTagsInStack) {}

  const char *what() const throw() {
    std::stringstream ss;
    ss << errorHeader() << ":Reached EOF while having " << remainingTagsInStack
       << " tags remaining in the stack";
    message = ss.str();
    return message.c_str();
  }
};

struct BadTagError : public SyntaxError {
  const std::string expectedTag;
  const std::string found;
  BadTagError(std::string file_name, size_t line, std::string expectedTag,
              std::string found)
      : SyntaxError(file_name, line), expectedTag(std::move(expectedTag)),
        found(std::move(found)) {}

  const char *what() const throw() {
    std::stringstream ss;
    ss << errorHeader() << ":Bad complete tag. Expected{" << expectedTag
       << "}. Found {" << found << "}.";
    message = ss.str();
    return message.c_str();
  }
};
