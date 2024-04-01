#pragma once

// Base classes for all node types
class Node {
public:
    virtual double evaluate() const = 0;   // Needs  to been overwriten
    virtual ~Node() {}
};


class BinaryOperationNode : public Node {
protected:
    Node* left;
    Node* right;
public:
    BinaryOperationNode(Node* l, Node* r) : left(l), right(r) {}

    virtual ~BinaryOperationNode() {        // Destructor
        delete left;
        delete right;
    }
};


class UnaryOperationNode : public Node {
protected:
    Node* value;
public:
    UnaryOperationNode(Node* v) : value(v) {}

    virtual ~UnaryOperationNode() {        // Destructor
        delete value;
    }
};



// Number node class
class NumberNode : public Node {
private:
    double value;
public:
    NumberNode(double val) : value(val) {}
    double evaluate() const override { return value; }
};

// Binary operation node base class


// Add node class
class AddNode : public BinaryOperationNode {
public:
    AddNode(Node* l, Node* r)  : BinaryOperationNode(l, r) {}
    double evaluate() const override { return left->evaluate() + right->evaluate(); }
};

// Subtract node class
class SubtractNode : public BinaryOperationNode {
public:
    SubtractNode(Node* l, Node* r) : BinaryOperationNode(l, r) {}
    double evaluate() const override { return left->evaluate() - right->evaluate(); }
};

// Multiply node class
class MultiplyNode : public BinaryOperationNode {
public:
    MultiplyNode(Node* l, Node* r) : BinaryOperationNode(l, r) {}
    double evaluate() const override { return left->evaluate() * right->evaluate(); }
};

// Divide node class
class DivideNode : public BinaryOperationNode {
public:
    DivideNode(Node* l, Node* r) : BinaryOperationNode(l, r) {}
    double evaluate() const override { return left->evaluate() / right->evaluate(); }
};

class NegateNode : public UnaryOperationNode {
public:
  NegateNode(Node* v): UnaryOperationNode(v) {}
  double evaluate() const override { return value->evaluate() * (-1); }
};
