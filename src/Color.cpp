#include "Color.h"

#include <iostream>

double Color::maxArray(double* array, int len) {
  double ret = array[0];
  for (int i = 0; i < len; i++)
    ret = max(ret, array[i]);
  return ret;
}
double Color::minArray(double* array, int len) {
  double ret = array[0];
  for (int i = 0; i < len; i++) 
    ret = min(ret, array[i]);
  return ret;
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
Color::Color(unsigned int red, unsigned int green, unsigned int blue): Value() {
  type = Value::COLOR;
  color[RGB_RED] = red;
  color[RGB_GREEN] = green;
  color[RGB_BLUE] = blue;
  
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
    stm << hex << (color[i] & 0xFF);
    sColor[i] = stm.str();
  }
  stm.str("");
  stm << "#";
  
  for (i = 0; i < 3; i++) {
    if (sColor[i].size() == 1)
      stm << "0";
    else if (sColor[i].size() > 2) 
      sColor[i] = sColor[i].substr(0, 2);
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
    color[RGB_RED] += c->getRed();
    color[RGB_GREEN] += c->getGreen();
    color[RGB_BLUE] += c->getBlue();
  } else if (v->type == PERCENTAGE) {
    percent = 1 + v->getPercent() * 0.01;
    color[RGB_RED] *= percent;
    color[RGB_GREEN] *= percent;
    color[RGB_BLUE] *= percent;
  } else 
    return false;
  return true;
}
bool Color::substract(Value* v) {
  Color* c;
  double percent;
  
  if (v->type == COLOR) {
    c = static_cast<Color*>(v);
    color[RGB_RED] -= c->getRed();
    color[RGB_GREEN] -= c->getGreen();
    color[RGB_BLUE] -= c->getBlue();
  } else if (v->type == PERCENTAGE) {
    percent = 1 - v->getPercent() * 0.01;
    cout << percent << endl;
    color[RGB_RED] *= percent;
    color[RGB_GREEN] *= percent;
    color[RGB_BLUE] *= percent;
  } else 
    return false;
  return true;
}
bool Color::multiply(Value* v) {
  if (v->type == NUMBER) {
    color[RGB_RED] *= v->getValue();
    color[RGB_GREEN] *= v->getValue();
    color[RGB_BLUE] *= v->getValue();
  } else 
    return false;
  return true;
}
bool Color::divide(Value* v) {
  if (v->type == NUMBER) {
    color[RGB_RED] /= v->getValue();
    color[RGB_GREEN] /= v->getValue();
    color[RGB_BLUE] /= v->getValue();
  } else 
    return false;
  return true;
}
    
void Color::setHSL(int hue, double saturation, double lightness) {
  double c, x, m;
  
  while (hue < 0) 
    hue = hue + 360;
  hue = hue % 360;
  cout << "set h: " << hue << ", s: " << saturation << ", l: " << lightness << endl;
  saturation = saturation * .01;
  lightness = lightness * .01;
  
  c = (1 - abs(2 * lightness - 1)) * saturation;
  hue = hue / 60;
  x = c * (1 - abs(hue % 2 - 1));

  if (hue < 1) {
    color[RGB_RED] = c;
    color[RGB_GREEN] = x;
    color[RGB_BLUE] = 0;
  } else if (hue < 2) {
    color[RGB_RED] = x;
    color[RGB_GREEN] = c;
    color[RGB_BLUE] = 0;
  } else if (hue < 3) {
    color[RGB_RED] = 0;
    color[RGB_GREEN] = c;
    color[RGB_BLUE] = x;
  } else if (hue < 4) {
    color[RGB_RED] = 0;
    color[RGB_GREEN] = x;
    color[RGB_BLUE] = c;
  } else if (hue < 5) {
    color[RGB_RED] = x;
    color[RGB_GREEN] = 0;
    color[RGB_BLUE] = c;
  } else if (hue < 6) {
    color[RGB_RED] = c;
    color[RGB_GREEN] = 0;
    color[RGB_BLUE] = x;
  }
  m = lightness - .5 * c;
  for (int i = 0; i < 3; i++) 
    color[i] = (color[i] + m) * 255;
}
void Color::lighten(double percent) {
  double* hsl = getHSL();
  setHSL(hsl[0], hsl[1] * 100, min(hsl[2] * 100 + percent, 100.00));
}
void Color::darken(double percent) {
  double* hsl = getHSL();
  setHSL(hsl[0], hsl[1] * 100, max(hsl[2] * 100 - percent, 0.0));
}
void Color::saturate(double percent) {
  double* hsl = getHSL();
  setHSL(hsl[0], min(hsl[1] * 100 + percent, 100.00), hsl[2]);
}
void Color::desaturate(double percent) {
  double* hsl = getHSL();
  setHSL(hsl[0], max(hsl[1] * 100 - percent, 0.00), hsl[2]);
}
void Color::fadein(double percent) {
}
void Color::fadeout(double percent) {
}
void Color::spin(double degrees) {
  double* hsl = getHSL();
  setHSL(hsl[0] + degrees, hsl[1] * 100, hsl[2] * 100);
}

unsigned int Color::getRed() {
  return color[RGB_RED];
}
unsigned int Color::getGreen() {
  return color[RGB_GREEN];
}
unsigned int Color::getBlue() {
  return color[RGB_BLUE];
}

double* Color::getHSL() {
  double max, min, c;
  double rgb[3], * hsl = new double[3];

  for (int i = 0; i < 3; i++)
    rgb[i] = (double)color[i] / 255;

  max = maxArray(rgb, 3);
  min = minArray(rgb, 3);
  c = max - min;
  
  if (c == 0)
    hsl[0] = 0;
  else if (max == rgb[RGB_RED]) {
    hsl[0] = (rgb[RGB_GREEN] - rgb[RGB_BLUE]) / c;
    while (hsl[0] > 6)
      hsl[0] = hsl[0] - 6;
  } else if (max == rgb[RGB_GREEN])
    hsl[0] = (rgb[RGB_BLUE] - rgb[RGB_RED]) / c + 2.0;
  else if (max == rgb[RGB_BLUE])
    hsl[0] = (rgb[RGB_RED] - rgb[RGB_GREEN]) / c + 4.0;
  hsl[0] = 60 * hsl[0];

  hsl[2] = .5 * (max + min);
  
  if (c == 0)
    hsl[1] = 0;
  else
    hsl[1] = c / (1 - abs(2 * hsl[2] - 1));

  cout << "get h: " << hsl[0] << ", s: " << hsl[1] << ", l: " << hsl[2] << endl;
  return hsl;
}

double Color::getHue() {
  double* hsl = getHSL();
  return hsl[0];
}
double Color::getSaturation() {
  double* hsl = getHSL();
  return hsl[1] * 100;
}
double Color::getLightness() {
  double* hsl = getHSL();
  return hsl[2] * 100;
}
