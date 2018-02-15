#include <iostream>
#include <sstream>

#include "less/value/Color.h"

#include "less/value/FunctionLibrary.h"

#define max(x, y) x > y ? x : y
#define min(x, y) x < y ? x : y

template <class T>
inline std::string to_string(const T& t) {
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


Color::Color(const Token &hash) : Value() {
  tokens.push_back(hash);
  token = hash;
  type = Value::COLOR;
  color_type = TOKEN;
  
  if (!parseHash(hash.c_str())) {
    throw new ValueException("A color value requires either three, four, "
                             "six or eight hexadecimal characters.",
                             tokens);
  }
}

Color::Color(const Token &name, const char* hash) : Value() {
  tokens.push_back(name);
  token = name;
  type = Value::COLOR;
  color_type = TOKEN;
  
  if (!parseHash(hash)) {
    cerr << "Hash for builtin color " <<
      name << "(" << hash << ") failed to parse." << endl;
  }
}


Color::Color() : Color((unsigned int)0,(unsigned int)0,(unsigned int)0) {
}

Color::Color(unsigned int red, unsigned int green, unsigned int blue)
  : Color(red, green, blue, 1) {
}

Color::Color(unsigned int red,
             unsigned int green,
             unsigned int blue,
             double alpha)
  : Value() {
  type = COLOR;
  color_type = RGB;
  rgb[RGB_RED] = red;
  rgb[RGB_GREEN] = green;
  rgb[RGB_BLUE] = blue;
  this->alpha = alpha;
}

Color::Color(double hue, double saturation, double lightness)
  : Color(hue, saturation, lightness, 1) {
}

Color::Color(double hue, double saturation, double lightness, double alpha)
  : Value() {

  type = COLOR;
  color_type = HSL;
  hsl[HSL_HUE] = hue;
  hsl[HSL_SATURATION] = saturation;
  hsl[HSL_LIGHTNESS] = lightness;
  this->alpha = alpha;
  HSLtoRGB();
}

Color::Color(const Color &color) : Value() {
  color_type = color.color_type;

  switch (color.color_type) {
  case TOKEN:
    token = color.token;    
    break;
    
  case RGB:
    break;
    
  case HSL:
    color.getHSL(hsl);
    break;
  }

  color.getRGB(rgb);
  
  alpha = color.getAlpha();
}

Color::~Color() {
}

bool Color::parseHash(const char* hash) {
  int len;

  alpha = 1;
  char hex[3] = "\0\0";

  switch (strlen(hash)) {
  case 5:
    alpha = 0;
  case 4:
    len = 1;
    break;
    
  case 9:
    alpha = 0;
  case 7:
    len = 2;
    break;
    
  default:
    return false;
    
  }
  hash++;
  
  if (alpha == 0) {
    memcpy(hex, hash, len);
    alpha = strtoul(hex, NULL, 16);
    if (len == 1)
      alpha = alpha * 0x11;
    hash += len;
  }

  for (int i = 0; i < 3; i++) {
    memcpy(hex, hash, len);
    rgb[i] = strtoul(hex, NULL, 16);
    if (len == 1)
      rgb[i] = rgb[i] * 0x11;
    hash += len;
  }
  return true;
}

Color* Color::fromName(const Token &name) {
  std::map<std::string, const char*>::iterator it;
  it = ColorNames.find(name);
  
  if (it != ColorNames.end())
    return new Color(name, (*it).second);
  else
    return NULL;
}

void Color::HSLtoRGB() {
  double _rgb[3], hue, saturation, lightness, c, x;
  int i;

  hue = hsl[HSL_HUE];
  while (hue < 0)
    hue += 360;
  while (hue > 360)
    hue -= 360;
  hue /= 360;
  saturation = hsl[HSL_SATURATION];
  lightness = hsl[HSL_LIGHTNESS];

  if (saturation > 0) {
    if (lightness < 0.5)
      c = lightness * (1.0 + saturation);
    else
      c = lightness + saturation - lightness * saturation;

    x = 2.0 * lightness - c;

    _rgb[RGB_RED] = hue + 1.0 / 3.0;
    _rgb[RGB_GREEN] = hue;
    _rgb[RGB_BLUE] = hue - 1.0 / 3.0;

    for (i = 0; i < 3; i++) {
      // bring into 0-1 range
      if (_rgb[i] > 1)
        _rgb[i] -= 1.0;
      if (_rgb[i] < 0)
        _rgb[i] += 1.0;

      if (6.0 * _rgb[i] < 1)
        _rgb[i] = x + (c - x) * 6.0 * _rgb[i];
      else if (2.0 * _rgb[i] < 1)
        _rgb[i] = c;
      else if (3.0 * _rgb[i] < 2)
        _rgb[i] = x + (c - x) * ((2.0 / 3.0) - _rgb[i]) * 6.0;
      else
        _rgb[i] = x;
    }
  } else
    _rgb[RGB_RED] = _rgb[RGB_GREEN] = _rgb[RGB_BLUE] = lightness;

  // convert to 0-255 range.
  // add the .5 and truncate to round to int.
  for (i = 0; i < 3; i++) {
    rgb[i] = _rgb[i] * 255 + 0.5;
  }
}

void Color::RGBtoHSL(double hsl[3]) const {
  double max, min, c;
  double _rgb[3];

  for (int i = 0; i < 3; i++)
    _rgb[i] = (double)rgb[i] / 255;

  max = maxArray(_rgb, 3);
  min = minArray(_rgb, 3);
  c = max - min;

  if (c == 0)
    hsl[HSL_HUE] = 0;
  else if (max == _rgb[RGB_RED]) {
    hsl[HSL_HUE] = (_rgb[RGB_GREEN] - _rgb[RGB_BLUE]) / c;
    while (hsl[HSL_HUE] > 6)
      hsl[HSL_HUE] = hsl[HSL_HUE] - 6;
  } else if (max == _rgb[RGB_GREEN])
    hsl[HSL_HUE] = (_rgb[RGB_BLUE] - _rgb[RGB_RED]) / c + 2.0;
  else if (max == _rgb[RGB_BLUE])
    hsl[HSL_HUE] = (_rgb[RGB_RED] - _rgb[RGB_GREEN]) / c + 4.0;
  hsl[HSL_HUE] = 60 * hsl[HSL_HUE];

  hsl[HSL_LIGHTNESS] = (max + min) / 2;

  if (c == 0)
    hsl[HSL_SATURATION] = 0;
  /* this part does not work */
  // else
  //  hsl[1] = c / (1.0 - abs(2.0 * hsl[2] - 1.0));
  else if (hsl[HSL_LIGHTNESS] < .5)
    hsl[HSL_SATURATION] = c / (max + min);
  else
    hsl[HSL_SATURATION] = c / (2.0 - max - min);
}

void Color::getRGB(unsigned int rgb[3]) const {
  rgb[RGB_RED] = this->rgb[RGB_RED];
  rgb[RGB_GREEN] = this->rgb[RGB_GREEN];
  rgb[RGB_BLUE] = this->rgb[RGB_BLUE];
}

void Color::getHSL(double hsl[3]) const {
  if (color_type == HSL) {

    hsl[HSL_HUE] = this->hsl[HSL_HUE];
    hsl[HSL_SATURATION] = this->hsl[HSL_SATURATION];
    hsl[HSL_LIGHTNESS] = this->hsl[HSL_LIGHTNESS];
  } else {
    RGBtoHSL(hsl);
  }
}

void Color::setRGB(unsigned int red,
                   unsigned int green,
                   unsigned int blue) {
  if (color_type != RGB) 
    color_type = RGB;
  
  rgb[RGB_RED] = min(red, 255);
  rgb[RGB_GREEN] = min(green, 255);
  rgb[RGB_BLUE] = min(blue, 255);
}
void Color::increaseRGB(int red,
                        int green,
                        int blue) {
  if (color_type != RGB) 
    color_type = RGB;
  
  rgb[RGB_RED] = min(rgb[RGB_RED] + red, 255);
  rgb[RGB_GREEN] = min(rgb[RGB_GREEN] + green, 255);
  rgb[RGB_BLUE] = min(rgb[RGB_BLUE] + blue, 255);
}
void Color::multiplyRGB(double red,
                        double green,
                        double blue) {
  if (color_type != RGB) 
    color_type = RGB;
  
  rgb[RGB_RED] = min(rgb[RGB_RED] * red, 255);
  rgb[RGB_GREEN] = min(rgb[RGB_GREEN] * green, 255);
  rgb[RGB_BLUE] = min(rgb[RGB_BLUE] * blue, 255);
}

void Color::lighten(double lightness) {
  if (color_type != HSL) {
    RGBtoHSL(hsl);
    color_type = HSL;
  }
  hsl[HSL_LIGHTNESS] += min(lightness, 1);
  HSLtoRGB();
}

void Color::darken(double lightness) {
  lighten(-lightness);
}

void Color::saturate(double saturation) {
  if (color_type != HSL) {
    RGBtoHSL(hsl);
    color_type = HSL;
  }
  hsl[HSL_SATURATION] += min(saturation, 1);
  HSLtoRGB();
}

void Color::desaturate(double saturation) {
  saturate(-saturation);
}

void Color::spin(double hue) {
  if (color_type != HSL) {
    RGBtoHSL(hsl);
    color_type = HSL;
  }
  hsl[HSL_HUE] += hue;
  HSLtoRGB();
}

void Color::setAlpha(double alpha) {
  this->alpha = alpha;
  color_type = RGB;
}
double Color::getAlpha() const {
  return alpha;
}

const TokenList* Color::getTokens() const {
  TokenList *tokens;
  ostringstream stm;
  string sColor[3];
  string hash;
  int i;

  if (color_type == TOKEN)
    return &this->tokens;

  tokens = new TokenList();

  // If the color is not opaque the rgba() function needs to be used.
  if (alpha < 1) {
    tokens->push_back(Token("rgba", Token::IDENTIFIER, 0, 0, "generated"));
    tokens->push_back(Token::BUILTIN_PAREN_OPEN);

    for (i = 0; i < 3; i++) {
      stm.str("");
      stm << (rgb[i] & 0xFF);
      tokens->push_back(Token(stm.str(), Token::NUMBER, 0, 0, "generated"));
      tokens->push_back(Token::BUILTIN_COMMA);
      tokens->push_back(Token::BUILTIN_SPACE);
    }
    stm.str("");
    stm << alpha;
    tokens->push_back(Token(stm.str(), Token::NUMBER, 0, 0, "generated"));
    tokens->push_back(Token::BUILTIN_PAREN_CLOSED);

  } else {
    for (i = 0; i < 3; i++) {
      stm.str("");
      stm << hex << (rgb[i] & 0xFF);
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
    if (hash[1] == hash[2] && hash[3] == hash[4] && hash[5] == hash[6]) {
      stm.str("");
      stm << "#" << hash[1] << hash[3] << hash[5];
      hash = stm.str();
    }

    tokens->push_back(Token(hash, Token::HASH, 0, 0, "generated"));
  }
  return tokens;
}


Value* Color::add(const Value& v) const {
  const Color* c;
  const NumberValue* n;
  const StringValue* s;
  unsigned int rgb[3];
  StringValue* ret;
  Color* cret;

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      cret = new Color(*this);
      cret->increaseRGB(rgb[RGB_RED], rgb[RGB_GREEN], rgb[RGB_BLUE]);
      return cret;
      
    case NUMBER:
    case PERCENTAGE:
    case DIMENSION:
      n = static_cast<const NumberValue*>(&v);
      cret = new Color(*this);
      cret->increaseRGB(n->getValue(), n->getValue(), n->getValue());
      return cret;

  case STRING:
      s = static_cast<const StringValue*>(&v);
      ret = new StringValue(*this, s->getQuotes());
      ret->append(*s);
      return ret;

    default:
      throw new ValueException(
          "You can only add colors with other \
colors, numbers or strings.",
          *this->getTokens());
  }
}
Value* Color::substract(const Value& v) const {
  const Color* c;
  const NumberValue* n;
  unsigned int rgb[3];
  Color *cret;

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      cret = new Color(*this);
      cret->increaseRGB(-rgb[RGB_RED], -rgb[RGB_GREEN], -rgb[RGB_BLUE]);
      return cret;
      
    case NUMBER:
    case PERCENTAGE:
    case DIMENSION:
      n = static_cast<const NumberValue*>(&v);
      cret = new Color(*this);
      cret->increaseRGB(-n->getValue(), -n->getValue(), -n->getValue());
      return cret;

  default:
      throw new ValueException(
          "You can only substract a color or \
a number from a color.",
          *this->getTokens());
  }
}

Value* Color::multiply(const Value& v) const {
  const Color* c;
  const NumberValue* n;
  unsigned int rgb[3];
  Color *cret;

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      cret = new Color(*this);
      cret->multiplyRGB(rgb[RGB_RED], rgb[RGB_GREEN], rgb[RGB_BLUE]);
      return cret;
      
    case NUMBER:
    case PERCENTAGE:
    case DIMENSION:
      n = static_cast<const NumberValue*>(&v);
      cret = new Color(*this);
      cret->multiplyRGB(n->getValue(), n->getValue(), n->getValue());
      return cret;

    default:
      throw new ValueException(
          "You can only multiply a color by a \
color or a number.",
          *this->getTokens());
  }
}
Value* Color::divide(const Value& v) const {
  const Color* c;
  const NumberValue* n;
  unsigned int rgb[3];
  Color *cret;

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      cret = new Color(*this);
      cret->multiplyRGB((double)1 / rgb[RGB_RED],
                        (double)1 / rgb[RGB_GREEN],
                        (double)1 / rgb[RGB_BLUE]);
      return cret;
    case NUMBER:
    case PERCENTAGE:
    case DIMENSION:
      n = static_cast<const NumberValue*>(&v);
      cret->multiplyRGB((double)1 / n->getValue(),
                        (double)1 / n->getValue(),
                        (double)1 / n->getValue());
      return cret;
    default:
      throw new ValueException(
          "You can only divide a color by a \
color or a number.",
          *this->getTokens());
  }
}

BooleanValue* Color::equals(const Value& v) const {
  const Color* c;
  const BooleanValue* b;
  unsigned int rgb[3];

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      
      return new BooleanValue(this->rgb[RGB_RED] == rgb[RGB_RED] &&
                              this->rgb[RGB_GREEN] == rgb[RGB_GREEN] &&
                              this->rgb[RGB_BLUE] == rgb[RGB_BLUE]);

  case BOOLEAN:
    // any color is falsy.
    b = static_cast<const BooleanValue*>(&v);
    return new BooleanValue(false == b->getValue());
    
  default:
      throw new ValueException("You can only compare a color with a *color*.",
                               *this->getTokens());
  }
}

BooleanValue* Color::lessThan(const Value& v) const {
  const Color* c;
  const BooleanValue* b;
  unsigned int rgb[3];

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      return new BooleanValue(this->rgb[RGB_RED] < rgb[RGB_RED] ||
                              this->rgb[RGB_GREEN] < rgb[RGB_GREEN] ||
                              this->rgb[RGB_BLUE] < rgb[RGB_BLUE]);
  case BOOLEAN:
    b = static_cast<const BooleanValue*>(&v);
    return new BooleanValue(b->getValue());

  default:
      throw new ValueException("You can only compare a color with a *color*.",
                               *this->getTokens());
  }
}



void Color::loadFunctions(FunctionLibrary& lib) {
  lib.push("rgb", "NNN", &Color::_rgb);
  lib.push("rgba", "NNN.", &Color::rgba);
  lib.push("lighten", "CP", &Color::_lighten);
  lib.push("darken", "CP", &Color::_darken);
  lib.push("saturate", "CP", &Color::_saturate);
  lib.push("desaturate", "CP", &Color::_desaturate);
  lib.push("fade", "CP", &Color::fade);
  lib.push("fadein", "CP", &Color::fadein);
  lib.push("fadeout", "CP", &Color::fadeout);
  lib.push("spin", "CN", &Color::spin);
  lib.push("hsl", "NPP", &Color::_hsl);
  lib.push("hsla", "NPP.", &Color::hsla);
  lib.push("hue", "C", &Color::hue);
  lib.push("saturation", "C", &Color::saturation);
  lib.push("lightness", "C", &Color::lightness);
  lib.push("argb", "C", &Color::argb);
  lib.push("red", "C", &Color::red);
  lib.push("blue", "C", &Color::blue);
  lib.push("green", "C", &Color::green);
  lib.push("alpha", "C", &Color::_alpha);
}

Value* Color::_rgb(const vector<const Value*>& arguments) {
  return new Color(
      (unsigned int)((const NumberValue*)arguments[0])->getValue(),
      (unsigned int)((const NumberValue*)arguments[1])->getValue(),
      (unsigned int)((const NumberValue*)arguments[2])->getValue());
}

Value* Color::rgba(const vector<const Value*>& arguments) {
  if (arguments[3]->type == Value::NUMBER) {
    return new Color(
        (unsigned int)((const NumberValue*)arguments[0])->getValue(),
        (unsigned int)((const NumberValue*)arguments[1])->getValue(),
        (unsigned int)((const NumberValue*)arguments[2])->getValue(),
        ((const NumberValue*)arguments[3])->getValue());
  } else if (arguments[3]->type == Value::PERCENTAGE) {
    return new Color(
        (unsigned int)((const NumberValue*)arguments[0])->getValue(),
        (unsigned int)((const NumberValue*)arguments[1])->getValue(),
        (unsigned int)((const NumberValue*)arguments[2])->getValue(),
        ((const NumberValue*)arguments[3])->getValue() * .01);
  } else {
    throw new ValueException(
        "Argument 3 needs to be a number "
        "or percentage.",
        *arguments[3]->getTokens());
  }
}
Value* Color::_lighten(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  c->lighten(value * .01);
  return c;
}
Value* Color::_darken(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  c->darken(value * .01);
  return c;
}

Value* Color::_saturate(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  c->saturate(value * .01);
  return c;
}
Value* Color::_desaturate(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  c->desaturate(value * .01);
  return c;
}

Value* Color::fade(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(value * .01);
  return c;
}

Value* Color::fadein(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(c->getAlpha() + (value * .01));
  return c;
}

Value* Color::fadeout(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(c->getAlpha() - (value * .01));
  return c;
}

Value* Color::spin(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  double degrees = ((const NumberValue*)arguments[1])->getValue();

  c->spin(degrees);
  return c;
}

Value* Color::_hsl(const vector<const Value*>& arguments) {
  return new Color(((const NumberValue*)arguments[0])->getValue(),
                   ((const NumberValue*)arguments[1])->getValue() * .01,
                   ((const NumberValue*)arguments[2])->getValue() * .01);
}

Value* Color::hsla(const vector<const Value*>& arguments) {
  double alpha;
  if (arguments[3]->type == Value::NUMBER) {
    alpha = ((const NumberValue*)arguments[3])->getValue();
  } else if (arguments[3]->type == Value::PERCENTAGE) {
    alpha = ((const NumberValue*)arguments[3])->getValue() * .01;
  } else {
    throw new ValueException(
                             "Argument 3 needs to be a number "
                             "or percentage.",
                             *arguments[3]->getTokens());
  }
  return new Color(((const NumberValue*)arguments[0])->getValue(),
                   ((const NumberValue*)arguments[1])->getValue() * .01,
                   ((const NumberValue*)arguments[2])->getValue() * .01,
                   alpha);
}

Value* Color::hue(const vector<const Value*>& arguments) {
  double hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_HUE]);
}

Value* Color::saturation(const vector<const Value*>& arguments) {
  double hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_SATURATION] * 100, Token::PERCENTAGE, NULL);
}

Value* Color::lightness(const vector<const Value*>& arguments) {
  double hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_LIGHTNESS] * 100, Token::PERCENTAGE, NULL);
}

Value* Color::argb(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];
  ostringstream stm;
  unsigned int color[4];
  string sColor[4];
  string hash;
  int i;
  Token t;

  color[0] = c->getAlpha() * 0xFF + 0.5;
  c->getRGB(color + 1);

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
  t = Token(hash, Token::STRING, 0, 0, "generated");
  return new StringValue(t, false);
}

Value* Color::red(const vector<const Value*>& arguments) {
  unsigned int rgb[3];
  ((const Color*)arguments[0])->getRGB(rgb);
  
  return new NumberValue(rgb[RGB_RED]);
}
Value* Color::blue(const vector<const Value*>& arguments) {
  unsigned int rgb[3];
  ((const Color*)arguments[0])->getRGB(rgb);
  
  return new NumberValue(rgb[RGB_BLUE]);
}
Value* Color::green(const vector<const Value*>& arguments) {
  unsigned int rgb[3];
  ((const Color*)arguments[0])->getRGB(rgb);
  
  return new NumberValue(rgb[RGB_GREEN]);
}
Value* Color::_alpha(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getAlpha());
}

std::map<string,const char*> Color::ColorNames = {
  {"black",		"#000000"},
  {"silver",		"#c0c0c0"},
  {"gray",		"#808080"},
  {"white",		"#ffffff"},
  {"maroon", 		"#800000"},
  {"red", 		"#ff0000"},
  {"purple", 		"#800080"},
  {"fuchsia",		"#ff00ff"},
  {"green", 		"#008000"},
  {"lime", 		"#00ff00"},
  {"olive", 		"#808000"},
  {"yellow", 		"#ffff00"},
  {"navy", 		"#000080"},
  {"blue", 		"#0000ff"},
  {"teal", 		"#008080"},
  {"aqua", 		"#00ffff"},
  {"orange", 		"#ffa500"},
  {"aliceblue", 	"#f0f8ff"},
  {"antiquewhite", 	"#faebd7"},
  {"aquamarine", 	"#7fffd4"},
  {"azure", 		"#f0ffff"},
  {"beige", 		"#f5f5dc"},
  {"bisque", 		"#ffe4c4"},
  {"blanchedalmond", 	"#ffebcd"},
  {"blueviolet", 	"#8a2be2"},
  {"brown", 		"#a52a2a"},
  {"burlywood", 	"#deb887"},
  {"cadetblue", 	"#5f9ea0"},
  {"chartreuse", 	"#7fff00"},
  {"chocolate", 	"#d2691e"},
  {"coral", 		"#ff7f50"},
  {"cornflowerblue", 	"#6495ed"},
  {"cornsilk", 	"#fff8dc"},
  {"crimson", 	"#dc143c"},
  {"cyan", 		"#00ffff"},
  {"darkblue", 	"#00008b"},
  {"darkcyan", 	"#008b8b"},
  {"darkgoldenrod", 	"#b8860b"},
  {"darkgray", 	"#a9a9a9"},
  {"darkgreen", 	"#006400"},
  {"darkgrey", 	"#a9a9a9"},
  {"darkkhaki", 	"#bdb76b"},
  {"darkmagenta", 	"#8b008b"},
  {"darkolivegreen", 	"#556b2f"},
  {"darkorange", 	"#ff8c00"},
  {"darkorchid", 	"#9932cc"},
  {"darkred", 	"#8b0000"},
  {"darksalmon", 	"#e9967a"},
  {"darkseagreen", 	"#8fbc8f"},
  {"darkslateblue", 	"#483d8b"},
  {"darkslategray", 	"#2f4f4f"},
  {"darkslategrey", 	"#2f4f4f"},
  {"darkturquoise", 	"#00ced1"},
  {"darkviolet", 	"#9400d3"},
  {"deeppink", 	"#ff1493"},
  {"deepskyblue", 	"#00bfff"},
  {"dimgray", 	"#696969"},
  {"dimgrey", 	"#696969"},
  {"dodgerblue", 	"#1e90ff"},
  {"firebrick", 	"#b22222"},
  {"floralwhite", 	"#fffaf0"},
  {"forestgreen", 	"#228b22"},
  {"gainsboro", 	"#dcdcdc"},
  {"ghostwhite", 	"#f8f8ff"},
  {"gold", 		"#ffd700"},
  {"goldenrod", 	"#daa520"},
  {"greenyellow", 	"#adff2f"},
  {"grey", 		"#808080"},
  {"honeydew", 	"#f0fff0"},
  {"hotpink", 	"#ff69b4"},
  {"indianred", 	"#cd5c5c"},
  {"indigo", 		"#4b0082"},
  {"ivory", 		"#fffff0"},
  {"khaki", 		"#f0e68c"},
  {"lavender", 	"#e6e6fa"},
  {"lavenderblush", 	"#fff0f5"},
  {"lawngreen", 	"#7cfc00"},
  {"lemonchiffon", 	"#fffacd"},
  {"lightblue", 	"#add8e6"},
  {"lightcoral", 	"#f08080"},
  {"lightcyan", 	"#e0ffff"},
  {"lightgoldenrodyellow", 	"#fafad2"},
  {"lightgray", 	"#d3d3d3"},
  {"lightgreen", 	"#90ee90"},
  {"lightgrey", 	"#d3d3d3"},
  {"lightpink", 	"#ffb6c1"},
  {"lightsalmon", 	"#ffa07a"},
  {"lightseagreen", 	"#20b2aa"},
  {"lightskyblue", 	"#87cefa"},
  {"lightslategray", 	"#778899"},
  {"lightslategrey", 	"#778899"},
  {"lightsteelblue", 	"#b0c4de"},
  {"lightyellow", 	"#ffffe0"},
  {"limegreen", 	"#32cd32"},
  {"linen", 		"#faf0e6"},
  {"magenta", 	"#ff00ff"},
  {"mediumaquamarine", 	"#66cdaa"},
  {"mediumblue", 	"#0000cd"},
  {"mediumorchid", 	"#ba55d3"},
  {"mediumpurple", 	"#9370db"},
  {"mediumseagreen", 	"#3cb371"},
  {"mediumslateblue",	"#7b68ee"},
  {"mediumspringgreen", 	"#00fa9a"},
  {"mediumturquoise",	"#48d1cc"},
  {"mediumvioletred",	"#c71585"},
  {"midnightblue", 	"#191970"},
  {"mintcream", 	"#f5fffa"},
  {"mistyrose", 	"#ffe4e1"},
  {"moccasin", 	"#ffe4b5"},
  {"navajowhite", 	"#ffdead"},
  {"oldlace", 	"#fdf5e6"},
  {"olivedrab", 	"#6b8e23"},
  {"orangered", 	"#ff4500"},
  {"orchid", 		"#da70d6"},
  {"palegoldenrod", 	"#eee8aa"},
  {"palegreen", 	"#98fb98"},
  {"paleturquoise", 	"#afeeee"},
  {"palevioletred", 	"#db7093"},
  {"papayawhip", 	"#ffefd5"},
  {"peachpuff", 	"#ffdab9"},
  {"peru", 		"#cd853f"},
  {"pink", 		"#ffc0cb"},
  {"plum", 		"#dda0dd"},
  {"powderblue", 	"#b0e0e6"},
  {"rosybrown", 	"#bc8f8f"},
  {"royalblue", 	"#4169e1"},
  {"saddlebrown", 	"#8b4513"},
  {"salmon", 		"#fa8072"},
  {"sandybrown", 	"#f4a460"},
  {"seagreen", 		"#2e8b57"},
  {"seashell", 		"#fff5ee"},
  {"sienna", 		"#a0522d"},
  {"skyblue", 		"#87ceeb"},
  {"slateblue", 	"#6a5acd"},
  {"slategray", 	"#708090"},
  {"slategrey", 	"#708090"},
  {"snow", 		"#fffafa"},
  {"springgreen", 	"#00ff7f"},
  {"steelblue", 	"#4682b4"},
  {"tan", 		"#d2b48c"},
  {"thistle", 		"#d8bfd8"},
  {"tomato", 		"#ff6347"},
  {"turquoise", 	"#40e0d0"},
  {"violet", 		"#ee82ee"},
  {"wheat", 		"#f5deb3"},
  {"whitesmoke", 	"#f5f5f5"},
  {"yellowgreen", 	"#9acd32"},
  {"rebeccapurple", 	"#663399"},
  {"transparent",	"#00000000"},
};
