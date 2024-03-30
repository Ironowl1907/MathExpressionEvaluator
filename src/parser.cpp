#include <cmath>
#include <vector> 
#include "../headers/parser.hpp"

#define log(x) std::cout << x << '\n'

Token Parser::at() {
    return Input[index];
}

int Parser::Parse(std::vector<Token> raw) {
    Input = raw;
    index = 0;
    return ParseExpr();
}

int Parser::ParseExpr(){
  int Term1 = ParseTerm();
  while (at().type == Sum || at().type == Rest) {
    TokenType factor = at().type;
    index ++;
    int Term2 = ParseTerm();
    Term1 = (factor == Sum )? Term1 + Term2 : Term1 - Term2;
  }
  return Term1;

}


int Parser::ParseTerm(){
  int Fac1 =  ParseFactor();
  while (at().type == Mul || at().type == Div) {
    TokenType factor = at().type;
    index ++;
    int Fac2  =ParseFactor();
    Fac1 = (factor == Mul )? Fac1 * Fac2 : Fac1 / Fac2;
  }
  return Fac1;
}


int Parser::ParseFactor(){
  if (at().type == Integer) {
    Token tempo = at();
    index++;
    return stoi(tempo.value);
  }
  else if (at().type == OpenPar){
    index++; // Consume (
    int expr = ParseExpr();
    if (at().type == ClosePar) index ++; // Consume )
    else std::cout << "[ERROR] Expected closing Parentesis" << '\n';
    return expr;
  }
  else {
    std::cout << "[ERROR] Expected integer, recived: " << at().value << '\n';
    return 0;
  }
}
