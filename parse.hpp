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
      : tokens(tokens), idx(0), token(&tokens[0]),
        filename(std::move(filename)) {}

  void buildSyntaxTree() {
    XML *xml = parseXML();
    expectToken(TokenKind::ENDOFFILE);
    std::cout << xml << '\n';
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
    while (!matchToken(TokenKind::LESS, TokenProcessKind::HALT)) {
      Expr *expr = new Expr();
      if (matchToken(TokenKind::ELEMENT, TokenProcessKind::HALT)) {
        expr->element = new Element(token->str);
        expr->kind = ExprKind::ELEMENT;
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
    while (token->kind == TokenKind::WHITESPACE) {
      nextToken();
    }
    expectToken(TokenKind::LESS);
    expectToken(TokenKind::ELEMENT, TokenProcessKind::HALT);
    tagStack.push_back(token->str);
    xml->tag = Tag(token->str);
    nextToken();
    if (!matchToken(TokenKind::GREAT_SLASH, TokenProcessKind::HALT)) {
      expectToken(TokenKind::GREAT);
      xml->exprBlock = parseExprBlock();
    } else {
      tagStack.pop_back();
      nextToken();
    }
    expectToken(TokenKind::LESS_SLASH);
    expectToken(TokenKind::ELEMENT, TokenProcessKind::HALT);
    if(token->str != tagStack.back()) {
      throw BadTagError(filename, 0, tagStack.back());
    }
    tagStack.pop_back();
    nextToken();
    expectToken(TokenKind::GREAT);
    while (token->kind == TokenKind::WHITESPACE) {
      nextToken();
    }
    return xml;
  }

  void nextToken() const {
    if (idx < tokens.size() - 1) {
      token = &tokens[++idx];
    } else if (!tagStack.empty()) {
      throw ReachedEOFError(filename, 0, tagStack.size());
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
      throw TokenNotFoundError(filename, 0, kind, token->kind);
    }
    if (process == TokenProcessKind::PASS) {
      nextToken();
    }
  }
};