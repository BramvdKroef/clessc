#include "Value.h"
#include <iostream>

Value::Value() {
}

Value::Value(Token* token) {
  tokens.push(token);
  
  switch(token->type) {
  case Token::NUMBER:
    type = NUMBER;
    break;
  case Token::PERCENTAGE:
    type = PERCENTAGE;
    break;
  case Token::DIMENSION:
    type = DIMENSION;
    break;
  default:
    throw new ParseException(token->str, "A number, percentage or dimension");
  }
}
Value::~Value() {
}

TokenList* Value::getTokens() {
  return &tokens;
}

bool Value::add(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() + v->getValue());
  else if (type == PERCENTAGE || type == DIMENSION)
    setValue(getValue() + v->getValue());
  else
    return false;
  
  return true;
}
bool Value::substract(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() - v->getValue());
  else if (type == PERCENTAGE && v->type == PERCENTAGE)
    setValue(getValue() - v->getValue());
  else
    return false;
  
  return true;
}
bool Value::multiply(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() * v->getValue());
  else if (type == PERCENTAGE && v->type == NUMBER)
    setValue(getValue() * v->getValue());
  else
    return false;
  
  return true;
}
bool Value::divide(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() / v->getValue());
  else if (type == PERCENTAGE && v->type == NUMBER)
    setValue(getValue() / v->getValue());
  else
    return false;
  
  return true;
}
double Value::getValue() {
  string number;
  istringstream stm;
  double ret;
  char c;

  for (unsigned int i = 0; i < tokens.front()->str.size(); i++) {
    c = tokens.front()->str[i];
    if (!isdigit(c) && c != '.' && c != '-') {
      number = tokens.front()->str.substr(0, i);
      break;
    }
  }
  if (number == "")
    number = tokens.front()->str;
  stm.str(number);
  stm >>ret;
  return ret;
}
string Value::getUnit () {
  char c;
  unsigned int i;
  
  for (i = 0; i < tokens.front()->str.size(); i++) {
    c = tokens.front()->str[i];
    if (!isdigit(c) && c != '.' && c != '-')
      return tokens.front()->str.substr(i);
  }
  return string("");
}

void Value::setValue(double d) {
  ostringstream stm;
  stm << d;
  if (type == Value::DIMENSION)
    stm << getUnit();
  else if (type == Value::PERCENTAGE)
    stm << "%";
  tokens.front()->str = stm.str();
}
