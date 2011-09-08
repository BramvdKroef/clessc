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

Color::Color(Token* token): Value() {
  int len;

  this->token = token;
  type = Value::COLOR;
  
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
      color[i] = color[i] * 0x11;
  }
}
Color::Color(unsigned int red, unsigned int green, unsigned int blue): Value(NULL) {
  type = Value::COLOR;
  color[0] = red;
  color[1] = green;
  color[2] = blue;
  
  token = new Token("#", Token::HASH);
}
Color::~Color() {
}

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
  stm << "#";
  
  for (i = 0; i < 3; i++) {
    if (sColor[i].size() == 1)
      stm << "0";
    stm << sColor[i];
  }
  token->str = stm.str();

  // convert to shorthand if possible
  if (token->str[1] == token->str[2] &&
      token->str[3] == token->str[4] &&
      token->str[5] == token->str[6]) {
    stm.str("");
    stm << "#" << token->str[1] << token->str[3] << token->str[4];
    token->str = stm.str();
  }
  return token;
}
bool Color::add(Value* v) {
  Color* c;
  double percent;
  
  if (v->type == COLOR) {
    c = static_cast<Color*>(v);
    color[0] += c->getRed();
    color[1] += c->getGreen();
    color[2] += c->getBlue();
  } else if (v->type == PERCENTAGE) {
    percent = v->getPercent() * 0.01;
    color[0] *= percent;
    color[1] *= percent;
    color[2] *= percent;
  } else 
    return false;
  return true;
}
bool Color::substract(Value* v) {
  Color* c;
  
  if (v->type == COLOR) {
    c = static_cast<Color*>(v);
    color[0] -= c->getRed();
    color[1] -= c->getGreen();
    color[2] -= c->getBlue();
  } else 
    return false;
  return true;
}
bool Color::multiply(Value* v) {
  if (v->type == NUMBER) {
    color[0] *= v->getValue();
    color[1] *= v->getValue();
    color[2] *= v->getValue();
  } else 
    return false;
  return true;
}
bool Color::divide(Value* v) {
  if (v->type == NUMBER) {
    color[0] /= v->getValue();
    color[1] /= v->getValue();
    color[2] /= v->getValue();
  } else 
    return false;
  return true;
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

unsigned int Color::getRed() {
  return color[0];
}
unsigned int Color::getGreen() {
  return color[1];
}
unsigned int Color::getBlue() {
  return color[2];
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
