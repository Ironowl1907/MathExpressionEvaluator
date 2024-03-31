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
  return 0; 
}

int Parser::ParseExpr(){
  return 0;
}


int Parser::ParseTerm(){
  return 0;
}


int  Parser::ParseFactor(){ 
  return 0; 
}


