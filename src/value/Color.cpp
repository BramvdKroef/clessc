/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

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

  this->tokens.push(token);
  valueChanged = false;
    
  type = Value::COLOR;
  
  if (token->str.size() == 4)
    len = 1;
  else if (token->str.size() == 7)
    len = 2;
  else {
    throw new ValueException("A color value requires either three or six hexadecimal characters.");
  }
  
  for (int i = 0; i < 3; i++) {
    istringstream stm(token->str.substr(1 + (i * len), len));
    stm >> hex >> color[i];
    if (len == 1)
      color[i] = color[i] * 0x11;
  }
  alpha = 1;
}
Color::Color(unsigned int red, unsigned int green, unsigned int blue): Value() {
  type = Value::COLOR;
  color[RGB_RED] = red;
  color[RGB_GREEN] = green;
  color[RGB_BLUE] = blue;
  alpha = 1;
  valueChanged = true;
}
Color::Color(unsigned int red, unsigned int green, unsigned int blue,
             double alpha): Value() { 
  type = Value::COLOR;
  color[RGB_RED] = red;
  color[RGB_GREEN] = green;
  color[RGB_BLUE] = blue;
  this->alpha = alpha;
  valueChanged = true;
}

Color::~Color() {
}

TokenList* Color::getTokens() {
  ostringstream stm;
  string sColor[3];
  string hash;
  int i;

  if (!valueChanged)
    return &tokens;

  tokens.clear();
  
  // If the color is not opaque the rgba() function needs to be used.
  if (alpha < 1) {
    tokens.push(new Token("rgba(", Token::FUNCTION));

    for (i = 0; i < 3; i++) {
      stm.str("");
      stm << (color[i] & 0xFF);
      tokens.push(new Token(stm.str(), Token::NUMBER));
      tokens.push(new Token(",", Token::OTHER));
    }
    stm.str("");
    stm << alpha;
    tokens.push(new Token(stm.str(), Token::NUMBER));
    tokens.push(new Token(")", Token::PAREN_CLOSED));
    valueChanged = false;
    return &tokens;
  }
  
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
  hash = stm.str();

  // convert to shorthand if possible
  if (hash[1] == hash[2] &&
      hash[3] == hash[4] &&
      hash[5] == hash[6]) {
    stm.str("");
    stm << "#" << hash[1] << hash[3] << hash[4];
    hash = stm.str();
  }

  tokens.push(new Token(hash, Token::HASH));
  valueChanged = false;
  return &tokens;
}
void Color::add(Value* v) {
  Color* c;
  NumberValue* n;
  
  if (v->type == COLOR) {
    c = static_cast<Color*>(v);
    color[RGB_RED] += c->getRed();
    color[RGB_GREEN] += c->getGreen();
    color[RGB_BLUE] += c->getBlue();
  } else if (v->type == Value::NUMBER ||
             v->type == Value::PERCENTAGE ||
             v->type == Value::DIMENSION) {
    n = static_cast<NumberValue*>(v);
    color[RGB_RED] += n->getValue();
    color[RGB_GREEN] += n->getValue();
    color[RGB_BLUE] += n->getValue();
  }
  valueChanged = true;
}
void Color::substract(Value* v) {
  Color* c;
  NumberValue* n;
  
  if (v->type == COLOR) {
    c = static_cast<Color*>(v);
    color[RGB_RED] = color[RGB_RED] > c->getRed() ?
      color[RGB_RED] - c->getRed() : 0;
    color[RGB_GREEN] = color[RGB_GREEN] > c->getGreen() ?
      color[RGB_GREEN] - c->getGreen() : 0;
    color[RGB_BLUE] = color[RGB_BLUE] > c->getBlue() ?
      color[RGB_BLUE] - c->getBlue() : 0;
  } else if (v->type == Value::NUMBER ||
             v->type == Value::PERCENTAGE ||
             v->type == Value::DIMENSION) {
    n = static_cast<NumberValue*>(v);
    color[RGB_RED] -= n->getValue();
    color[RGB_GREEN] -= n->getValue();
    color[RGB_BLUE] -= n->getValue();
  }
  valueChanged = true;
}
void Color::multiply(Value* v) {
  Color* c;
  NumberValue* n;
  int result;
  
  if (v->type == COLOR) {
    c = static_cast<Color*>(v);
    result = color[RGB_RED] * c->getRed();
    color[RGB_RED] = max(min(result, 255), 0);
    result = color[RGB_GREEN] * c->getGreen();
    color[RGB_GREEN] = max(min(result, 255), 0);
    result = color[RGB_BLUE] * c->getBlue();
    color[RGB_BLUE] = max(min(result, 255), 0);
  } else if (v->type == Value::NUMBER ||
             v->type == Value::PERCENTAGE ||
             v->type == Value::DIMENSION) {
    n = static_cast<NumberValue*>(v);
    color[RGB_RED] *= n->getValue();
    color[RGB_GREEN] *= n->getValue();
    color[RGB_BLUE] *= n->getValue();
  }
  valueChanged = true;
}
void Color::divide(Value* v) {
  Color* c;
  NumberValue* n;
  
  if (v->type == COLOR) {
    c = static_cast<Color*>(v);
    color[RGB_RED] /= c->getRed();
    color[RGB_GREEN] /= c->getGreen();
    color[RGB_BLUE] /= c->getBlue();
  } else if (v->type == Value::NUMBER ||
             v->type == Value::PERCENTAGE ||
             v->type == Value::DIMENSION){
    n = static_cast<NumberValue*>(v);
    color[RGB_RED] /= n->getValue();
    color[RGB_GREEN] /= n->getValue();
    color[RGB_BLUE] /= n->getValue();
  }
  valueChanged = true;
}
    
void Color::setHSL(double hue, double saturation, double lightness) {
  double c, x, rgb[3];
  int i;
  
  while (hue < 0) 
    hue += 360;
  while (hue > 360)
    hue -= 360;
  hue /= 360;
  saturation *= .01;
  lightness *= .01;

  if (saturation > 0) {
    if (lightness < 0.5)
      c = lightness * (1.0 + saturation);
    else
      c = lightness + saturation - lightness * saturation;

    x = 2.0 * lightness - c;

    rgb[RGB_RED] = hue + 1.0/3.0;
    rgb[RGB_GREEN] = hue;
    rgb[RGB_BLUE] = hue - 1.0/3.0;

    for (i = 0; i < 3; i++) {
      // bring into 0-1 range
      if (rgb[i] > 1)
        rgb[i] -= 1.0;
      if (rgb[i] < 0)
        rgb[i] += 1.0;

      if (6.0 * rgb[i] < 1) 
        rgb[i] = x + (c - x) * 6.0 * rgb[i];
      else if (2.0 * rgb[i] < 1)
        rgb[i] = c;
      else if (3.0 * rgb[i] < 2)
        rgb[i] = x + (c - x) * ((2.0/3.0) - rgb[i]) * 6.0;
      else
        rgb[i] = x;
    }
  } else
    rgb[RGB_RED] = rgb[RGB_GREEN] = rgb[RGB_BLUE] = lightness;
  
  for (i = 0; i < 3; i++) {
    // convert to 0-255 range.
    // add the .5 and truncate to round to int.
    color[i] = rgb[i] * 255 + 0.5;
  }
  valueChanged = true;
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
  setHSL(hsl[0], min(hsl[1] * 100 + percent, 100.00), hsl[2] * 100);
}
void Color::desaturate(double percent) {
  double* hsl = getHSL();
  setHSL(hsl[0], max(hsl[1] * 100 - percent, 0.00), hsl[2] * 100);
}
void Color::fadein(double percent) {
  alpha *= min(1 + percent * .01, 100.00);
  valueChanged = true;
}
void Color::fadeout(double percent) {
  alpha *= max(1 - percent * .01, 0.00);
  valueChanged = true;
}
void Color::spin(double degrees) {
  double* hsl = getHSL();
  setHSL((int)hsl[0] + degrees, hsl[1] * 100, hsl[2] * 100);
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

  hsl[2] = (max + min) / 2;
    
  if (c == 0)
    hsl[1] = 0;
  /* this part does not work */
  //else
  //  hsl[1] = c / (1.0 - abs(2.0 * hsl[2] - 1.0));
  else if (hsl[2] < .5)
    hsl[1] = c / (max + min);
  else
    hsl[1] = c / (2.0 - max - min);
  return hsl;
}

int Color::getHue() {
  double* hsl = getHSL();
  return hsl[0];
}
int Color::getSaturation() {
  double* hsl = getHSL();
  return hsl[1] * 100;
}
int Color::getLightness() {
  double* hsl = getHSL();
  return hsl[2] * 100;
}
