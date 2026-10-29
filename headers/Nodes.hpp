#pragma once

#include <exception>
#include <iostream>
#include <map>

using SymbolTable = std::map<char, double>;

class EvaluationError : public std::exception {
public:
  const char *what() const noexcept override {
    return "Evaluation error";
  }
};

// Base classes for all node types
class Node {
public:
  virtual double evaluate(const SymbolTable &symbols) const = 0;
  virtual ~Node() {}
};

class BinaryOperationNode : public Node {
protected:
  Node *left;
  Node *right;

public:
  BinaryOperationNode(Node *l, Node *r) : left(l), right(r) {}

  virtual ~BinaryOperationNode() {
    delete left;
    delete right;
  }
};

class UnaryOperationNode : public Node {
protected:
  Node *value;

public:
  UnaryOperationNode(Node *v) : value(v) {}

  virtual ~UnaryOperationNode() {
    delete value;
  }
};

// Number node class
class NumberNode : public Node {
private:
  double value;

public:
  NumberNode(double val) : value(val) {}
  double evaluate(const SymbolTable &symbols) const override { return value; }
};

// Variable node class
class VariableNode : public Node {
private:
  char name;

public:
  VariableNode(char n) : name(n) {}
  double evaluate(const SymbolTable &symbols) const override {
    auto it = symbols.find(name);
    if (it == symbols.end()) {
      std::cout << "[Evaluation ERROR] Undefined variable '" << name << "'\n";
      throw EvaluationError();
    }
    return it->second;
  }
};

// Add node class
class AddNode : public BinaryOperationNode {
public:
  AddNode(Node *l, Node *r) : BinaryOperationNode(l, r) {}
  double evaluate(const SymbolTable &symbols) const override {
    return left->evaluate(symbols) + right->evaluate(symbols);
  }
};

// Subtract node class
class SubtractNode : public BinaryOperationNode {
public:
  SubtractNode(Node *l, Node *r) : BinaryOperationNode(l, r) {}
  double evaluate(const SymbolTable &symbols) const override {
    return left->evaluate(symbols) - right->evaluate(symbols);
  }
};

// Multiply node class
class MultiplyNode : public BinaryOperationNode {
public:
  MultiplyNode(Node *l, Node *r) : BinaryOperationNode(l, r) {}
  double evaluate(const SymbolTable &symbols) const override {
    return left->evaluate(symbols) * right->evaluate(symbols);
  }
};

// Divide node class
class DivideNode : public BinaryOperationNode {
public:
  DivideNode(Node *l, Node *r) : BinaryOperationNode(l, r) {}
  double evaluate(const SymbolTable &symbols) const override {
    return left->evaluate(symbols) / right->evaluate(symbols);
  }
};

class NegateNode : public UnaryOperationNode {
public:
  NegateNode(Node *v) : UnaryOperationNode(v) {}
  double evaluate(const SymbolTable &symbols) const override {
    return value->evaluate(symbols) * (-1);
  }
};
