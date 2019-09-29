#pragma once

#include <exception>

#include "astnode.hpp"
#include "exception.hpp"
#include "token.hpp"

struct Parser {
public:
  const std::vector<Token> &tokens;
  const std::string filename;

  Parser(std::string filename, const std::vector<Token> &tokens)
      : tokens(tokens), filename(std::move(filename)), idx(0), token(&tokens[0])
         {}

  XML* buildSyntaxTree() {
    XML* xml = nullptr;
    try {
      xml = parseXML();
      expectToken(TokenKind::ENDOFFILE, TokenProcessKind::HALT);
      if(!tagStack.empty()) {
        throw UnClosedTagError(filename, token->line, tagStack.size(), tagStack.back());
      }
    } catch (const SyntaxError &se) {
      std::cerr << se.what() << std::endl;
      std::cerr << idx << std::endl;
    }
    return xml;
  }

private:
  mutable size_t idx;
  mutable const Token *token;
  mutable std::vector<std::string> tagStack;

  enum class TokenProcessKind {
    HALT,
    PASS,
  };

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

  XML *parseXML() {
    XML *xml = new XML();
    if (token->kind == TokenKind::WHITESPACE) {
      nextToken();
    }
    expectToken(TokenKind::LESS);
    expectToken(TokenKind::ELEMENT, TokenProcessKind::HALT);
    tagStack.push_back(token->str);
    xml->tag = Tag(token->str);
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

  void nextToken() const {
    if (idx < tokens.size() - 1) {
      token = &tokens[++idx];
    } else if (!tagStack.empty()) {
      throw ReachedEOFError(filename, token->line, tagStack.size());
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