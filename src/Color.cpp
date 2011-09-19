#include "Color.h"

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
Color::Color(unsigned int red, unsigned int green, unsigned int blue): Value() {
  type = Value::COLOR;
  color[RED] = red;
  color[GREEN] = green;
  color[BLUE] = blue;
  
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
    color[RED] += c->getRed();
    color[GREEN] += c->getGreen();
    color[BLUE] += c->getBlue();
  } else if (v->type == PERCENTAGE) {
    percent = 1 + v->getPercent() * 0.01;
    color[RED] *= percent;
    color[GREEN] *= percent;
    color[BLUE] *= percent;
  } else 
    return false;
  return true;
}
bool Color::substract(Value* v) {
  Color* c;
  double percent;
  
  if (v->type == COLOR) {
    c = static_cast<Color*>(v);
    color[RED] -= c->getRed();
    color[GREEN] -= c->getGreen();
    color[BLUE] -= c->getBlue();
  } else if (v->type == PERCENTAGE) {
    percent = 1 - v->getPercent() * 0.01;
    cout << percent << endl;
    color[RED] *= percent;
    color[GREEN] *= percent;
    color[BLUE] *= percent;
  } else 
    return false;
  return true;
}
bool Color::multiply(Value* v) {
  if (v->type == NUMBER) {
    color[RED] *= v->getValue();
    color[GREEN] *= v->getValue();
    color[BLUE] *= v->getValue();
  } else 
    return false;
  return true;
}
bool Color::divide(Value* v) {
  if (v->type == NUMBER) {
    color[RED] /= v->getValue();
    color[GREEN] /= v->getValue();
    color[BLUE] /= v->getValue();
  } else 
    return false;
  return true;
}
    
void Color::setHSL(int hue, unsigned int saturation,
                   unsigned int lightness) {
  double c, x, m;
  
  while (hue < 0) 
    hue = 360 - hue;
  hue = hue % 360;
  
  saturation = saturation * .01;
  lightness = lightness * .01;
  
  c = (1 - abs(2 * lightness - 1)) * saturation;
  hue = hue / 60;
  x = c * (1 - abs(hue % 2 - 1));

  if (hue < 1) {
    color[RED] = c;
    color[GREEN] = x;
    color[BLUE] = 0;
  } else if (hue < 2) {
    color[RED] = x;
    color[GREEN] = c;
    color[BLUE] = 0;
  } else if (hue < 3) {
    color[RED] = 0;
    color[GREEN] = c;
    color[BLUE] = x;
  } else if (hue < 4) {
    color[RED] = 0;
    color[GREEN] = x;
    color[BLUE] = c;
  } else if (hue < 5) {
    color[RED] = x;
    color[GREEN] = 0;
    color[BLUE] = c;
  } else if (hue < 6) {
    color[RED] = c;
    color[GREEN] = 0;
    color[BLUE] = x;
  }
  m = lightness - .5 * c;
  for (int i = 0; i < 3; i++)
    color[i] = color[i] + m;
}
void Color::lighten(unsigned int percent) {
  unsigned double* hsl = getHSL();
  setHSL(hsl[0], hsl[1] * 100, max(hsl[2] * 100 + percent, 100));
}
void Color::darken(unsigned int percent) {
  unsigned double* hsl = getHSL();
  setHSL(hsl[0], hsl[1] * 100, min(hsl[2] * 100 - percent, 0));
}
void Color::saturate(unsigned int percent) {
  unsigned double* hsl = getHSL();
  setHSL(hsl[0], max(hsl[1] * 100 + percent, 100), hsl[2]);
}
void Color::desaturate(unsigned int percent) {
  unsigned double* hsl = getHSL();
  setHSL(hsl[0], max(hsl[1] * 100 - percent, 100), hsl[2]);
}
void Color::fadein(unsigned int percent) {
}
void Color::fadeout(unsigned int percent) {
}
void Color::spin(int percent) {
  unsigned double* hsl = getHSL();
  setHSL(getHue() + percent, getSaturation(), getLightness());
}

unsigned int Color::getRed() {
  return color[RED];
}
unsigned int Color::getGreen() {
  return color[GREEN];
}
unsigned int Color::getBlue() {
  return color[BLUE];
}

unsigned double* Color::getHSL() {
  unsigned double max, min, c;
  unsigned double rgb[3], hsl[3];

  for (int i = 0; i < 3; i++)
    rgb[i] = color[i] / 255;
  
  max = max(rgb);
  min = min(rgb);
  c = max - min;
  
  if (c == 0)
    hsl[0] = 0;
  else if (max == color[RED])
    hsl[0] = (color[GREEN] - color[BLUE]) / c % 6;
  else if (max == color[GREEN])
    hsl[0] = (color[BLUE] - color[RED]) / c + 2;
  else if (max == color[BLUE])
    hsl[0] = (color[RED] - color[GREEN]) / c + 4;
  hsl[0] = 60 * hsl[0];

  hsl[2] = .5 * (max + min);
  
  if (c == 0)
    hsl[1] = 0;
  else
    hsl[1] = c / (1 - abs(2 * hsl[2] - 1));
  return hsl;
}

unsigned double Color::getHue() {
  unsigned double* hsl = getHSL();
  return hsl[0];
}
unsigned double Color::getSaturation() {
  unsigned double* hsl = getHSL();
  return hsl[1] * 100;
}
unsigned double Color::getLightness() {
  unsigned double* hsl = getHSL();
  return hsl[2] * 100;
}
