#pragma once

#include <cassert>
#include <ostream>
#include <string>
#include <vector>

struct XML;

struct Attribute {
  std::string name;
  std::string value;
  Attribute(std::string name, std::string value)
      : name(std::move(name)), value(std::move(value)) {}
};

struct Tag {
  std::string name;
  std::vector<Attribute *> attrs;
  Tag(std::string name) : name(std::move(name)) {}
  Tag() : name() {}
  ~Tag();
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
  ~Expr();
};

struct ExprBlock {
  std::vector<Expr *> exprs;
  ~ExprBlock();
};

struct XML {
  Tag *tag;
  ExprBlock *exprBlock;

  XML() : tag(nullptr), exprBlock(nullptr) {}
  ~XML();
};

Tag::~Tag() {
  for (auto &attr : attrs) {
    delete attr;
  }
}

Expr::~Expr() {
  if (kind == ExprKind::XML) {
    delete xml;
  } else {
    delete element;
  }
}

ExprBlock::~ExprBlock() {
  for (auto &expr : exprs) {
    delete expr;
  }
}

XML::~XML() {
  delete tag;
  delete exprBlock;
}

std::ostream &operator<<(std::ostream &os, const XML *xml);

std::ostream &operator<<(std::ostream& os, const Attribute* attr) {
  return os << "[Attr name:" << attr->name << " value:" << attr->value << "]";
}

std::ostream &operator<<(std::ostream &os, const Tag *tag) {
  os << "[Tag name:" << tag->name << ' ';
  for(const auto& attr: tag->attrs) {
    os << attr << ' ';
  }
  return os << "]";
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
  os << "[XML tag:" << xml->tag << " expr-block:";
  if (xml->exprBlock) {
    os << xml->exprBlock;
  } else {
    os << "null";
  }
  return os << "]";
};