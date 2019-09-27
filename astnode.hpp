#pragma once

#include <cassert>
#include <ostream>
#include <string>
#include <vector>

struct XML;

struct Tag {
  std::string name;
  // std::vector<Attribute*> attrs;
  Tag(std::string name) : name(std::move(name)) {}
  Tag() : name() {}
};

struct Element {
  std::string elem;
  Element(std::string elem) : elem(std::move(elem)) {}
};

enum class ExprKind {
  XML,
  ELEMENT,
};

struct Expr {
  ExprKind kind;
  union {
    XML *xml;
    Element *element;
  };
  Expr() : xml(nullptr) {}
};

struct ExprBlock {
  std::vector<Expr *> exprs;
};

struct XML {
  Tag tag;
  ExprBlock *exprBlock;

  XML() : tag(), exprBlock(nullptr) {}
};

std::ostream &operator<<(std::ostream &os, const XML *xml);

std::ostream &operator<<(std::ostream &os, const Tag &tag) {
  return os << "[Tag name:" << tag.name << "]";
}

std::ostream &operator<<(std::ostream &os, const Element *element) {
  return os << "[Element elem:" << element->elem << "]";
}

std::ostream &operator<<(std::ostream &os, const Expr *expr) {
  if (expr) {
    switch (expr->kind) {
    case ExprKind::ELEMENT: {
      os << "[Expr kind:Element element:" << expr->element << "]";
      break;
    }
    case ExprKind::XML: {
      os << "[Expr kind:XML xml:" << expr->xml << "]";
      break;
    }
    default: { assert(0); }
    }
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const ExprBlock *exprBlock) {
  for (const auto &expr : exprBlock->exprs) {
    os << expr << '\n';
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const XML *xml) {
  return os << "[XML tag:" << xml->tag << " expr-block:" << xml->exprBlock
            << "]";
};