#include "Value.h"

Value::Value() {
}

Value::Value(Token* token) {
  this->token = token;
  
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
  if (token != NULL)
    delete token;
}

Token* Value::getToken() {
  return token;
}

bool Value::add(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() + v->getValue());
  else if (type == PERCENTAGE && v->type == PERCENTAGE)
    setPercent(getPercent() + v->getPercent());
  else
    return false;
  
  return true;
}
bool Value::substract(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() - v->getValue());
  else if (type == PERCENTAGE && v->type == PERCENTAGE)
    setPercent(getPercent() - v->getPercent());
  else
    return false;
  
  return true;
}
bool Value::multiply(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() * v->getValue());
  else if (type == PERCENTAGE && v->type == NUMBER)
    setPercent(getPercent() * v->getValue());
  else
    return false;
  
  return true;
}
bool Value::divide(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() / v->getValue());
  else if (type == PERCENTAGE && v->type == NUMBER)
    setPercent(getPercent() / v->getValue());
  else
    return false;
  
  return true;
}
double Value::getValue() {
  istringstream stm;
  double ret;
  stm.str(token->str);
  stm >>ret;
  return ret;
}
string Value::getUnit () {
  char c;
  unsigned int i;
  
  for (i = 0; i < token->str.size(); i++) {
    c = token->str[i];
    if (!isdigit(c) && c != '.' && c != '-')
      return token->str.substr(i);
  }
  return string("");
}

int Value::getPercent() {
  if (type != Value::PERCENTAGE)
    return -1;
  else
    return (int)getValue();
}
void Value::setValue(double d) {
  ostringstream stm;
  stm << d;
  if (type == Value::DIMENSION)
    stm << getUnit();
  else if (type == Value::PERCENTAGE)
    stm << "%";
  token->str = stm.str();
}
void Value::setPercent(int i) {
  ostringstream stm;
  if (type != Value::PERCENTAGE)
    return;

  stm << i;
  stm << "%";
  token->str = stm.str();
}

