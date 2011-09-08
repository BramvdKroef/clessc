#include "Value.h"

#include <iostream>

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

Color::Color(Token* token): Value() {
  unsigned int color[3];
  int len;

  this->token = token;
  
  if (token->str.size() == 4)
    len = 1;
  else if (token->str.size() == 7)
    len = 2;
  else {
    throw new ParseException(token->str,
                             "Either three or six hexadecimal characters.");
  }
  
  for (int i = 0; i < 3; i++) {
    istringstream stm(token->str.substr(1 + (i * len), len));
    stm >> hex >> color[i];
    if (len == 1)
      color[i] = color[i] * 11;
  }
  Color(color[0], color[1], color[2]);
}
Color::Color(unsigned int red, unsigned int green, unsigned int blue): Value(NULL) {
  type = Value::COLOR;
  color[0] = red;
  color[1] = green;
  color[2] = blue;
  
  if (token == NULL)
    token = new Token("#", Token::HASH);
}
Color::~Color() {}

Token* Color::getToken() {
  ostringstream stm;
  string sColor[3];
  int i;
  
  for (i = 0; i < 3; i++) {
    stm.str("");
    stm << hex << color[i];
    sColor[i] = stm.str();
  }
  stm.str("");
  
  if (sColor[0].size() == 1 && sColor[1].size() == 1 &&
      sColor[2].size() == 1) {
    stm << "#" << sColor[0] << sColor[1] << sColor[2];
  } else {
    stm << "#";
    for (i = 0; i < 3; i++) {
      if (sColor[i].size() == 1)
        stm << "0";
      stm << sColor[i];
    }
  }
  token->str = stm.str();
  return token;
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
