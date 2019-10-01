#pragma once

#include <exception>

#include "astnode.hpp"
#include "exception.hpp"
#include "lex.hpp"
#include "token.hpp"

enum class TokenProcessKind {
  HALT,
  PASS,
};

struct Parser {
public:
  const std::vector<Token> &tokens;
  const std::string filename;

  Parser(std::string filename, const std::vector<Token> &tokens)
      : tokens(tokens), filename(std::move(filename)), idx(0),
        token(&tokens[0]) {}

protected:
  mutable size_t idx;
  mutable const Token *token;

  void nextToken() const {
    if (idx < tokens.size() - 1) {
      token = &tokens[++idx];
    }
  }

  bool matchToken(TokenKind kind,
                  TokenProcessKind process = TokenProcessKind::PASS) const {
    if (token->kind == kind) {
      if (process == TokenProcessKind::PASS) {
        nextToken();
      }
      return true;
    }
    return false;
  }

  void expectToken(TokenKind kind,
                   TokenProcessKind process = TokenProcessKind::PASS) const {
    if (!matchToken(kind, TokenProcessKind::HALT)) {
      throw TokenNotFoundError(filename, token->line, kind, token->kind);
    }
    if (process == TokenProcessKind::PASS) {
      nextToken();
    }
  }
};

struct TagParser : public Parser {
public:
  TagParser(std::string filename, const std::vector<Token> &tokens)
      : Parser(filename, tokens) {}

  Tag *buildTag() {
    Tag *tag = nullptr;
    try {
      tag = parseTag();
      expectToken(TokenKind::ENDOFFILE, TokenProcessKind::HALT);
    } catch (const SyntaxError &se) {
      std::cerr << se.what() << std::endl;
    }
    return tag;
  }

private:
  Tag *parseTag() {
    Tag *tag = new Tag();
    expectToken(TokenKind::NAME, TokenProcessKind::HALT);
    tag->name = token->str;
    nextToken();
    while (matchToken(TokenKind::NAME, TokenProcessKind::HALT)) {
      const std::string &attrName = token->str;
      nextToken();
      expectToken(TokenKind::EQUALS);
      expectToken(TokenKind::DOUBLE_QUOTED_STRING, TokenProcessKind::HALT);
      const std::string &attrVal = token->str;
      tag->attrs.push_back(new Attribute(attrName, attrVal));
      nextToken();
    }
    nextToken();
    return tag;
  }
};

struct XMLParser : public Parser {
  XMLParser(std::string filename, const std::vector<Token> &tokens)
      : Parser(std::move(filename), std::move(tokens)) {}

  XML *buildSyntaxTree() {
    XML *xml = nullptr;
    try {
      xml = parseXML();
      expectToken(TokenKind::ENDOFFILE, TokenProcessKind::HALT);
      if (!tagStack.empty()) {
        throw UnClosedTagError(filename, token->line, tagStack.size(),
                               tagStack.back());
      }
    } catch (const SyntaxError &se) {
      std::cerr << se.what() << std::endl;
      std::cerr << idx << std::endl;
    }
    return xml;
  }

private:
  mutable std::vector<std::string> tagStack;

  ExprBlock *parseExprBlock() {
    ExprBlock *exprBlock = new ExprBlock();
    while (matchToken(TokenKind::LESS, TokenProcessKind::HALT) ||
           matchToken(TokenKind::ELEMENT, TokenProcessKind::HALT) ||
           matchToken(TokenKind::WHITESPACE)) {
      Expr *expr = new Expr();
      if (matchToken(TokenKind::ELEMENT, TokenProcessKind::HALT)) {
        expr->element = new Element(token->str);
        expr->kind = ExprKind::ELEMENT;
        nextToken();
      } else {
        expr->xml = parseXML();
        expr->kind = ExprKind::XML;
      }
      exprBlock->exprs.push_back(expr);
    }
    return exprBlock;
  }

  Tag *parseTag() {
    TagLexer tl(filename, token->str, 0, 0);
    tl.scan();
    TokenHolder &th = tl.tokens;
    TagParser tp(filename, th.container);
    Tag *tag = tp.buildTag();
    tagStack.push_back(tag->name);
    return tag;
  }

  XML *parseXML() {
    XML *xml = new XML();
    if (token->kind == TokenKind::WHITESPACE) {
      nextToken();
    }
    expectToken(TokenKind::LESS);
    expectToken(TokenKind::ELEMENT, TokenProcessKind::HALT);
    xml->tag = parseTag();
    nextToken();
    if (matchToken(TokenKind::GREAT_SLASH, TokenProcessKind::HALT)) {
      tagStack.pop_back();
      nextToken();
    } else {
      expectToken(TokenKind::GREAT);
      xml->exprBlock = parseExprBlock();
      expectToken(TokenKind::LESS_SLASH);
      expectToken(TokenKind::ELEMENT, TokenProcessKind::HALT);
      if (token->str != tagStack.back()) {
        throw BadTagError(filename, token->line, tagStack.back(), token->str);
      }
      tagStack.pop_back();
      nextToken();
      expectToken(TokenKind::GREAT);
    }
    if (token->kind == TokenKind::WHITESPACE) {
      nextToken();
    }
    return xml;
  }
};