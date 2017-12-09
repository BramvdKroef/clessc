#include <less/value/Color.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

#include <sstream>
#include <iostream>

#define max(x,y) x > y ? x : y
#define min(x,y) x < y ? x : y


template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

double Color::maxArray(double* array, const size_t len) const {
  double ret = array[0];
  for (size_t i = 1; i < len; i++)
    ret = max(ret, array[i]);
  return ret;
}
double Color::minArray(double* array, const size_t len) const {
  double ret = array[0];
  for (size_t i = 1; i < len; i++) 
    ret = min(ret, array[i]);
  return ret;
}

void Color::updateTokens() {
  ostringstream stm;
  string sColor[3];
  string hash;
  int i;

  tokens.clear();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Update tokens";
#endif

  // If the color is not opaque the rgba() function needs to be used.
  if (alpha < 1) {
    tokens.push_back(Token("rgba", Token::IDENTIFIER, 0, 0, "generated"));
    tokens.push_back(Token::BUILTIN_PAREN_OPEN);

    for (i = 0; i < 3; i++) {
      stm.str("");
      stm << (color[i] & 0xFF);
      tokens.push_back(Token(stm.str(), Token::NUMBER, 0, 0, "generated"));
      tokens.push_back(Token::BUILTIN_COMMA);
    }
    stm.str("");
    stm << alpha;
    tokens.push_back(Token(stm.str(), Token::NUMBER, 0, 0, "generated"));
    tokens.push_back(Token::BUILTIN_PAREN_CLOSED);

  } else {
  
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
      stm << "#" << hash[1] << hash[3] << hash[5];
      hash = stm.str();
    }

    tokens.push_back(Token(hash, Token::HASH, 0,0,"generated"));
  }
#ifdef WITH_LIBGLOG
  VLOG(3) << tokens.toString();
#endif

}

Color::Color(): Value() {
  type = Value::COLOR;
  color[RGB_RED] = 0;
  color[RGB_GREEN] = 0;
  color[RGB_BLUE] = 0;
  alpha = 1.0;
  updateTokens();
}

Color::Color(Token &token): Value() {
  int len;

  this->tokens.push_back(token);
    
  type = Value::COLOR;
  
  if (token.size() == 4)
    len = 1;
  else if (token.size() == 7)
    len = 2;
  else {
    throw new ValueException("A color value requires either three "
                             "or six hexadecimal characters.", *this->getTokens());
  }
  
  for (int i = 0; i < 3; i++) {
    istringstream stm(token.substr(1 + (i * len), len));
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
  updateTokens();
}
Color::Color(unsigned int red, unsigned int green, unsigned int blue,
             double alpha): Value() { 
  type = Value::COLOR;
  color[RGB_RED] = red;
  color[RGB_GREEN] = green;
  color[RGB_BLUE] = blue;
  this->alpha = alpha;
  updateTokens();
}

Color* Color::fromHSL(double hue, double saturation, double lightness) {
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
  
  // convert to 0-255 range.
  // add the .5 and truncate to round to int.

  return new Color(rgb[RGB_RED] * 255 + 0.5,
                   rgb[RGB_GREEN] * 255 + 0.5,
                   rgb[RGB_BLUE] * 255 + 0.5);
}

Color::Color(const Color &color) {
  type = Value::COLOR;
  this->color[RGB_RED] = color.getRed();
  this->color[RGB_GREEN] = color.getGreen();
  this->color[RGB_BLUE] = color.getBlue();
  alpha = color.getAlpha();
  updateTokens();
}

Color::~Color() {
}

Value* Color::add(const Value &v) const {
  const Color* c;
  const NumberValue* n;
  const StringValue* s;
  StringValue* ret;
  
  switch (v.type) {
  case COLOR:
    c = static_cast<const Color*>(&v);
    return new Color(min(color[RGB_RED] + c->getRed(), 255),
                     min(color[RGB_GREEN] + c->getGreen(), 255),
                     min(color[RGB_BLUE] + c->getBlue(), 255));
  case NUMBER:
  case PERCENTAGE:
  case DIMENSION:
    n = static_cast<const NumberValue*>(&v);
    return new Color(min(color[RGB_RED] + n->getValue(), 255),
                     min(color[RGB_GREEN] + n->getValue(), 255),
                     min(color[RGB_BLUE] + n->getValue(), 255));

  case STRING:
    s = static_cast<const StringValue*>(&v);
    ret = new StringValue(*this, s->getQuotes());
    ret->append(*s);
    return ret;
    
  default:
    throw new ValueException("You can only add colors with other \
colors, numbers or strings.", *this->getTokens());
  }
}
Value* Color::substract(const Value &v) const {
  const Color* c;
  const NumberValue* n;
  
  switch (v.type) {
  case COLOR:
    c = static_cast<const Color*>(&v);
    return new Color((color[RGB_RED] > c->getRed() ?
                      color[RGB_RED] - c->getRed() : 0),
                     (color[RGB_GREEN] > c->getGreen() ?
                      color[RGB_GREEN] - c->getGreen() : 0),
                     (color[RGB_BLUE] > c->getBlue() ?
                      color[RGB_BLUE] - c->getBlue() : 0));

  case NUMBER:
  case PERCENTAGE:
  case DIMENSION:
    n = static_cast<const NumberValue*>(&v);

    return new Color(max(color[RGB_RED] - n->getValue(), 0),
                     max(color[RGB_GREEN] - n->getValue(), 0),
                     max(color[RGB_BLUE] - n->getValue(), 0));
  default:
    throw new ValueException("You can only substract a color or \
a number from a color.", *this->getTokens());
  }
}

Value* Color::multiply(const Value &v) const {
  const Color* c;
  const NumberValue* n;
  
  switch (v.type) {
  case COLOR:
    c = static_cast<const Color*>(&v);

    return new Color(min(color[RGB_RED] * c->getRed(), 255),
                     min(color[RGB_GREEN] * c->getGreen(), 255),
                     min(color[RGB_BLUE] * c->getBlue(), 255));
  case NUMBER:
  case PERCENTAGE:
  case DIMENSION:
    n = static_cast<const NumberValue*>(&v);
    return new Color(min(color[RGB_RED] * n->getValue(), 255),
                     min(color[RGB_GREEN] * n->getValue(), 255),
                     min(color[RGB_BLUE] * n->getValue(), 255));

  default:
    throw new ValueException("You can only multiply a color by a \
color or a number.", *this->getTokens());
  }
}
Value* Color::divide(const Value &v) const {
  const Color* c;
  const NumberValue* n;
  
  switch (v.type) {
  case COLOR:
    c = static_cast<const Color*>(&v);
    return new Color(color[RGB_RED] / c->getRed(),
                     color[RGB_GREEN] / c->getGreen(),
                     color[RGB_BLUE] / c->getBlue());
  case NUMBER:
  case PERCENTAGE:
  case DIMENSION:
    n = static_cast<const NumberValue*>(&v);
    return new Color(color[RGB_RED] / n->getValue(),
                     color[RGB_GREEN] / n->getValue(),
                     color[RGB_BLUE] / n->getValue());
  default:
    throw new ValueException("You can only divide a color by a \
color or a number.", *this->getTokens());
  }
}

BooleanValue* Color::equals(const Value &v) const {
  const Color* c;

  switch(v.type) {
  case COLOR:
    c = static_cast<const Color*>(&v);
    return new BooleanValue(color[RGB_RED] == c->getRed() &&
                            color[RGB_GREEN] == c->getGreen() &&
                            color[RGB_BLUE] == c->getBlue());
  default:
    throw new ValueException("You can only compare a color with a *color*.", *this->getTokens());

  }
}

BooleanValue* Color::lessThan(const Value &v) const {
  const Color* c;

  switch(v.type) {
  case COLOR:
    c = static_cast<const Color*>(&v);
    return new BooleanValue(color[RGB_RED] < c->getRed() ||
                            color[RGB_GREEN] < c->getGreen() ||
                            color[RGB_BLUE] < c->getBlue());
  default:
    throw new ValueException("You can only compare a color with a *color*.", *this->getTokens());

  }
}
    


void Color::setRGB(unsigned int red, unsigned int green, unsigned int blue) {
  color[RGB_RED] = red;
  color[RGB_GREEN] = green;
  color[RGB_BLUE] = blue;
  updateTokens();
}
void Color::setAlpha(double alpha) {
  this->alpha = min(max(alpha, 0.0), 1.0);
  updateTokens();
}
double Color::getAlpha() const {
  return alpha;
}

unsigned int Color::getRed() const {
  return color[RGB_RED];
}
unsigned int Color::getGreen() const {
  return color[RGB_GREEN];
}
unsigned int Color::getBlue() const {
  return color[RGB_BLUE];
}

double* Color::getHSL() const {
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


void Color::loadFunctions(FunctionLibrary &lib) {
  lib.push("rgb", "NNN", &Color::rgb);
  lib.push("rgba", "NNN.", &Color::rgba);
  lib.push("lighten", "CP", &Color::lighten);
  lib.push("darken", "CP", &Color::darken);
  lib.push("saturate", "CP", &Color::saturate);
  lib.push("desaturate", "CP", &Color::desaturate);
  lib.push("fadein", "CP", &Color::fadein);
  lib.push("fadeout", "CP", &Color::fadeout);
  lib.push("spin", "CN", &Color::spin);
  lib.push("hsl", "NPP", &Color::hsl);
  lib.push("hue", "C", &Color::hue);
  lib.push("saturation", "C", &Color::saturation);
  lib.push("lightness", "C", &Color::lightness);
  lib.push("argb", "C", &Color::argb);
  lib.push("red", "C", &Color::red);
  lib.push("blue", "C", &Color::blue);
  lib.push("green", "C", &Color::green);
  lib.push("alpha", "C", &Color::_alpha);
}

Value* Color::rgb(const vector<const Value*> &arguments) {
  return new Color((unsigned int)((const NumberValue*)arguments[0])->getValue(),
                   (unsigned int)((const NumberValue*)arguments[1])->getValue(),
                   (unsigned int)((const NumberValue*)arguments[2])->getValue());
}

Value* Color::rgba(const vector<const Value*> &arguments) {
  if (arguments[3]->type == Value::NUMBER) {
    return new Color((unsigned int)((const NumberValue*)arguments[0])->getValue(),
                     (unsigned int)((const NumberValue*)arguments[1])->getValue(),
                     (unsigned int)((const NumberValue*)arguments[2])->getValue(),
                     ((const NumberValue*)arguments[3])->getValue());
  } else if (arguments[3]->type == Value::PERCENTAGE) {
    return new Color((unsigned int)((const NumberValue*)arguments[0])->getValue(),
                     (unsigned int)((const NumberValue*)arguments[1])->getValue(),
                     (unsigned int)((const NumberValue*)arguments[2])->getValue(),
                     ((const NumberValue*)arguments[3])->getValue() * .01);
  } else {
    throw new ValueException("Argument 3 needs to be a number "
                             "or percentage.",
                             *arguments[3]->getTokens());
  }
}
Value* Color::lighten(const vector<const Value*> &arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double value = ((const NumberValue*)arguments[1])->getValue();

  return Color::fromHSL(hsl[0], hsl[1] * 100,
                        min(hsl[2] * 100 + value, 100.00));
}
Value* Color::darken(const vector<const Value*> &arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double value = ((const NumberValue*)arguments[1])->getValue();
  
  return Color::fromHSL(hsl[0], hsl[1] * 100,
                         max(hsl[2] * 100 - value, 0.00));
}
Value* Color::saturate(const vector<const Value*> &arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double value = ((const NumberValue*)arguments[1])->getValue();

  return Color::fromHSL(hsl[0],
                        min(hsl[1] * 100 + value, 100.00),
                        hsl[2] * 100);
}
Value* Color::desaturate(const vector<const Value*> &arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double value = ((const NumberValue*)arguments[1])->getValue();
  
  return Color::fromHSL(hsl[0],
                        max(hsl[1] * 100 - value, 0.00),
                        hsl[2] * 100);
}

Value* Color::fadein(const vector<const Value*> &arguments) {
  const Color* c = static_cast<const Color*>(arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();
  
  Color* ret = new Color(c->getRed(),
                         c->getGreen(),
                         c->getBlue(),
                         c->getAlpha() + value * .01);
  return ret;
}

Value* Color::fadeout(const vector<const Value*> &arguments) {
  const Color* c = static_cast<const Color*>(arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();
  
  Color* ret = new Color(c->getRed(),
                         c->getGreen(),
                         c->getBlue(),
                         c->getAlpha() - value * .01);
  return ret;
}

Value* Color::spin(const vector<const Value*> &arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double degrees = ((const NumberValue*)arguments[1])->getValue();

  return Color::fromHSL(std::floor(hsl[0] + degrees),
                         hsl[1] * 100,
                         hsl[2] * 100);
}

Value* Color::hsl(const vector<const Value*> &arguments) {
  return Color::fromHSL(((const NumberValue*)arguments[0])->getValue(),
                        ((const NumberValue*)arguments[1])->getValue(),
                        ((const NumberValue*)arguments[2])->getValue());
}

Value* Color::hue(const vector<const Value*> &arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();

  return new NumberValue(hsl[0]);
}

Value* Color::saturation(const vector<const Value*> &arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();

  return new NumberValue(hsl[1] * 100, Token::PERCENTAGE, NULL);
}

Value* Color::lightness(const vector<const Value*> &arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();

  return new NumberValue(hsl[2] * 100, Token::PERCENTAGE, NULL);
}

Value* Color::argb(const vector<const Value*> &arguments) {
  const Color* c = (const Color*)arguments[0];
  ostringstream stm;
  unsigned int color[4];
  string sColor[4];
  string hash;
  int i;
  Token t;

  color[0] = c->getAlpha() * 0xFF + 0.5;
  color[1] = c->getRed();
  color[2] = c->getGreen();
  color[3] = c->getBlue();
  
  for (i = 0; i < 4; i++) {
    stm.str("");
    stm << hex << (color[i] & 0xFF);
    sColor[i] = stm.str();
  }
  stm.str("");
  stm << "#";
  
  for (i = 0; i < 4; i++) {
    if (sColor[i].size() == 1)
      stm << "0";
    else if (sColor[i].size() > 2) 
      sColor[i] = sColor[i].substr(0, 2);
    stm << sColor[i];
  }
  hash = stm.str();
  t = Token(hash, Token::STRING, 0,0,"generated");
  return new StringValue(t, false);
}

Value* Color::red(const vector<const Value*> &arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getRed());
}
Value* Color::blue(const vector<const Value*> &arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getBlue());
}
Value* Color::green(const vector<const Value*> &arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getGreen());
}
Value* Color::_alpha(const vector<const Value*> &arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getAlpha());
}
