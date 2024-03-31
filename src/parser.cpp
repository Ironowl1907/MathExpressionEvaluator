#include <cmath>
#include <vector> 
#include "../headers/parser.hpp"


Token Parser::at() {
    return Input[index];
}

Token Parser::peak(){
  return Input[index+1];
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


int  Parser::ParseFactor(){ 
  if (at().type == OpenPar){
    index++; // Consume (
    int expr = ParseExpr();
    if (at().type == ClosePar) index ++; // Consume )
    else std::cout << "[ERROR] Expected closing Parentesis at: " << index << '\n';
    return expr;
  }
  if (at().type == Rest && peak().type == OpenPar){
    index+=2; // Consume - and (
    int expr = ParseExpr();
    if (at().type == ClosePar) index ++; // Consume )
    else std::cout << "[ERROR] Expected closing Parentesis at: " << index << '\n';
    return expr * (-1);

  }
  else {
    return ParsePrefix();
  }
}

int Parser::ParsePrefix(){

  if (at().type == Rest){
    index ++;
    Token temp = at();
    index ++;
    return stoi (temp.value) * (-1);
  }
  else if (at ().type == Integer){
    Token temp = at();
    index ++;
    return stoi(temp.value);
  }
  else {
    std::cout << "[ERROR] Expected Factor, recived: " << at().value << '\n';
    return 0;
  }
}
