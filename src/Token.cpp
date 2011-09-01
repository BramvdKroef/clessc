#include "Token.h"

Token::Token () {
  type = OTHER;
}
Token::Token (string s, Type t) {
  str = s;
  type = t;
}
  
void Token::add(char c) {
  str.append(1, c);
}
  
void Token::clear () {
  str.clear();
  type = OTHER;
}
bool Token::equals (Token* t) {
  return (type == t->type && str == t->str);
}
Token* Token::clone () {
  return new Token(str, type);
}
