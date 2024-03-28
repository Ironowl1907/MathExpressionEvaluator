
#include <vector> 
#include "../headers/parser.hpp"

Token Parser::at() {
    return Input[index];
}

int Parser::Parse(std::vector<Token> raw) {
    Input = raw;
    index = 0;
    return ParseExpr();
}

int Parser::ParseExpr() {
    int Fac1 = ParseTerm();
    index++;
    while (at().type == Sum || at().type == Rest) {
        index++;
        int Fac2 = ParseTerm();
        Fac1 = (at().type == Sum) ? Fac1 + Fac2 : Fac1 - Fac2;
    }
    return Fac1;
} 

int Parser::ParseTerm() {
    int Fac1 = ParseFactor();
    index++;
    while (at().type == Mul || at().type == Div) {
        index++;
        int Fac2 = ParseFactor();
        Fac1 = (at().type == Mul) ? Fac1 * Fac2 : Fac1 / Fac2;
    }
    return Fac1;
} 

int Parser::ParseFactor() {
    Token tempo = at();
    index++;
    return tempo.value;
}
