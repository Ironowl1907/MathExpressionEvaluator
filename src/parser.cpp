#include <vector> 
#include "../headers/parser.hpp"
#include "../headers/Nodes.hpp"

#define log(x) std::cout << x << '\n'

Token Parser::at() {
  return (index >= Input.size())? token(Error) : Input[index];
}

Token Parser::peak(){
  return (index + 1 >= Input.size())? token(Error) : Input[ index + 1 ];
}

Node* Parser::Parse(std::vector<Token> raw) {
  Input = raw;
  index = 0;
  Node* par = ParseExpr();
  if (par == nullptr) {
    return nullptr;
  }
  return par;
}

Node* Parser::ParseExpr(){
    Node* Term1 = ParseTerm();
    while (at().type == Sum || at().type == Rest){
        TokenType Operator = at().type; 
        index++;     // Eat Operator
        Node* Term2 = ParseTerm();
        if(Term2 == nullptr) return nullptr;      // Checks for not null pointers
        if (Operator == Sum)
            Term1 = new AddNode(Term1, Term2); // Assign the new node to Term1
        else
            Term1 = new SubtractNode(Term1, Term2); // Assign the new node to Term1
    }
    return Term1;
}

Node* Parser::ParseTerm(){
    Node* Fac1 = ParseFactor();
    while (at().type == Div || at().type == Mul){
        TokenType Operator = at().type; 
        index++;     // Eat Operator
        Node* Fac2 = ParseFactor();
        if(Fac2 == nullptr) return nullptr;  // Checks for not null pointers
        if (Operator == Mul)
            Fac1 = new MultiplyNode(Fac1, Fac2); // Assign the new node to Fac1
        else
            Fac1 = new DivideNode(Fac1, Fac2); // Assign the new node to Fac1
    }
    return Fac1;
}

Node* Parser::ParseFactor(){
    if (at().type == Integer){
        Token temp = at();
        index ++;
        return new NumberNode(stoi(temp.value));
    }
    if (at().type == OpenPar){
        index ++;   // Eating par Token 
        Node* expr = ParseExpr();
        if (at().type != ClosePar){
          std::cout << "[Parsing ERROR] Expected closing parentheses at line: " << index << '\n';
          return nullptr;
        }
        index ++;
        return expr;
    }
    if (at().type == Rest && peak().type == Integer){
      index ++; 
      Token temp = at();
      index ++;
      return  new NumberNode(stoi(temp.value) * (-1));
    }

    if (at().type == Rest && peak().type == OpenPar){
          index += 2; 
          Node* expr = ParseExpr();
          if (at().type != ClosePar){
            std::cout << "[Parsing ERROR] Expected closing parentheses at line: " << index << '\n';
            return nullptr;
          }
          return  new NegateNode(expr);
    }

    else{
        std::cout << "[Parsing ERROR] Expected Factor (aka. Number or (Expression)), instead received: '" << at().value << "'"<< '\n';
        return nullptr;
    }
}
