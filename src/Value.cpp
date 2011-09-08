#include "Value.h"

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
  }
}
Value::~Value() {
}

Token* Value::getToken() {
  return token;
}

bool Value::add(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() + v->getValue());
  else if (type == PERCENTAGE && v->type == PERCENTAGE)
    setValue(getPercent() + v->getPercent());
  else
    return false;
  
  return true;
}
bool Value::substract(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() - v->getValue());
  else if (type == PERCENTAGE && v->type == PERCENTAGE)
    setValue(getPercent() - v->getPercent());
  else
    return false;
  
  return true;
}
bool Value::multiply(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() * v->getValue());
  else if (type == PERCENTAGE && v->type == PERCENTAGE)
    setValue(getPercent() * v->getPercent());
  else
    return false;
  
  return true;
}
bool Value::divide(Value* v) {
  if (type == NUMBER && v->type == NUMBER)
    setValue(getValue() * v->getValue());
  else if (type == PERCENTAGE && v->type == PERCENTAGE)
    setValue(getPercent() * v->getPercent());
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
  
  for (int i = 0; i < token->str.size(); i++) {
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
  istringstream stm;
  stm << d;
  if (type == Value::DIMENSION)
    stm << getUnit();
  else if (type == Value::PERCENTAGE)
    stm << "%";
  token.str = stm.str();
}

Color::Color(Token* token): Value(token) {
  type = Value::COLOR;
}
Color::Color(unsigned int red, unsigned int green, unsigned int blue): Value(NULL) {
  type = Value::COLOR;
}
Color::~Color() {}

Token* Color::getToken() {
  return NULL;
}
  
void Color::setHSL(unsigned int hue, unsigned int saturation, unsigned int
            lightness) {
}
void Color::lighten(unsigned int percent) {
}
void Color::darken(unsigned int percent) {
}
void Color::saturate(unsigned int percent) {
}
void Color::desaturate(unsigned int percent) {
}
void Color::fadein(unsigned int percent) {
}
void Color::fadeout(unsigned int percent) {
}
void Color::spin(unsigned int percent) {
}

int Color::getHue() {
  return 0;
}
int Color::getSaturation() {
  return 0;
}
int Color::getLightness() {
  return 0;
}
