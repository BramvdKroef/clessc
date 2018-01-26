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

void Color::updateTokens() {
  ostringstream stm;
  string sColor[3];
  string hash;
  int i;

  tokens.clear();

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
    if (hash[1] == hash[2] && hash[3] == hash[4] && hash[5] == hash[6]) {
      stm.str("");
      stm << "#" << hash[1] << hash[3] << hash[5];
      hash = stm.str();
    }

    tokens.push_back(Token(hash, Token::HASH, 0, 0, "generated"));
  }
}

Color::Color(const Token &hash) : Value() {
  tokens.push_back(hash);
  type = Value::COLOR;
  
  if (!parseHash(hash.c_str())) {
    throw new ValueException("A color value requires either three, four, "
                             "six or eight hexadecimal characters.",
                             tokens);
  }
}

Color::Color(const Token &name, const char* hash) : Value() {
  tokens.push_back(name);
  type = Value::COLOR;
  
  if (!parseHash(hash)) {
    cerr << "Hash for builtin color " <<
      name << "(" << hash << ") failed to parse." << endl;
  }
}


Color::Color() : Value() {
  type = Value::COLOR;
  color[RGB_RED] = 0;
  color[RGB_GREEN] = 0;
  color[RGB_BLUE] = 0;
  alpha = 1.0;
  updateTokens();
}

Color::Color(unsigned int red, unsigned int green, unsigned int blue)
    : Value() {
  type = Value::COLOR;
  color[RGB_RED] = red;
  color[RGB_GREEN] = green;
  color[RGB_BLUE] = blue;
  alpha = 1;
  updateTokens();
}

Color::Color(unsigned int red,
             unsigned int green,
             unsigned int blue,
             double alpha)
    : Value() {
  type = Value::COLOR;
  color[RGB_RED] = red;
  color[RGB_GREEN] = green;
  color[RGB_BLUE] = blue;
  this->alpha = alpha;
  updateTokens();
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
    color[i] = strtoul(hex, NULL, 16);
    if (len == 1)
      color[i] = color[i] * 0x11;
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

    rgb[RGB_RED] = hue + 1.0 / 3.0;
    rgb[RGB_GREEN] = hue;
    rgb[RGB_BLUE] = hue - 1.0 / 3.0;

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
        rgb[i] = x + (c - x) * ((2.0 / 3.0) - rgb[i]) * 6.0;
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

Color::Color(const Color& color) {
  type = Value::COLOR;
  this->color[RGB_RED] = color.getRed();
  this->color[RGB_GREEN] = color.getGreen();
  this->color[RGB_BLUE] = color.getBlue();
  alpha = color.getAlpha();
  updateTokens();
}

Color::~Color() {
}

Value* Color::add(const Value& v) const {
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
      throw new ValueException(
          "You can only add colors with other \
colors, numbers or strings.",
          *this->getTokens());
  }
}
Value* Color::substract(const Value& v) const {
  const Color* c;
  const NumberValue* n;

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      return new Color(
          (color[RGB_RED] > c->getRed() ? color[RGB_RED] - c->getRed() : 0),
          (color[RGB_GREEN] > c->getGreen() ? color[RGB_GREEN] - c->getGreen()
                                            : 0),
          (color[RGB_BLUE] > c->getBlue() ? color[RGB_BLUE] - c->getBlue()
                                          : 0));

    case NUMBER:
    case PERCENTAGE:
    case DIMENSION:
      n = static_cast<const NumberValue*>(&v);

      return new Color(max(color[RGB_RED] - n->getValue(), 0),
                       max(color[RGB_GREEN] - n->getValue(), 0),
                       max(color[RGB_BLUE] - n->getValue(), 0));
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
      throw new ValueException(
          "You can only multiply a color by a \
color or a number.",
          *this->getTokens());
  }
}
Value* Color::divide(const Value& v) const {
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
      throw new ValueException(
          "You can only divide a color by a \
color or a number.",
          *this->getTokens());
  }
}

BooleanValue* Color::equals(const Value& v) const {
  const Color* c;
  const BooleanValue* b;

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      return new BooleanValue(color[RGB_RED] == c->getRed() &&
                              color[RGB_GREEN] == c->getGreen() &&
                              color[RGB_BLUE] == c->getBlue());

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

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      return new BooleanValue(color[RGB_RED] < c->getRed() ||
                              color[RGB_GREEN] < c->getGreen() ||
                              color[RGB_BLUE] < c->getBlue());
  case BOOLEAN:
    b = static_cast<const BooleanValue*>(&v);
    return new BooleanValue(b->getValue());

  default:
      throw new ValueException("You can only compare a color with a *color*.",
                               *this->getTokens());
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
  double rgb[3], *hsl = new double[3];

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
  // else
  //  hsl[1] = c / (1.0 - abs(2.0 * hsl[2] - 1.0));
  else if (hsl[2] < .5)
    hsl[1] = c / (max + min);
  else
    hsl[1] = c / (2.0 - max - min);
  return hsl;
}

void Color::loadFunctions(FunctionLibrary& lib) {
  lib.push("rgb", "NNN", &Color::rgb);
  lib.push("rgba", "NNN.", &Color::rgba);
  lib.push("lighten", "CP", &Color::lighten);
  lib.push("darken", "CP", &Color::darken);
  lib.push("saturate", "CP", &Color::saturate);
  lib.push("desaturate", "CP", &Color::desaturate);
  lib.push("fade", "CP", &Color::fade);
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

Value* Color::rgb(const vector<const Value*>& arguments) {
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
Value* Color::lighten(const vector<const Value*>& arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double value = ((const NumberValue*)arguments[1])->getValue();
  Color *ret = Color::fromHSL(hsl[0],
                               hsl[1] * 100,
                               min(hsl[2] * 100 + value, 100.00));
  ret->setAlpha(((const Color*)arguments[0])->getAlpha());
  return ret;
}
Value* Color::darken(const vector<const Value*>& arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double value = ((const NumberValue*)arguments[1])->getValue();
  Color *ret = Color::fromHSL(hsl[0],
                              hsl[1] * 100,
                              max(hsl[2] * 100 - value, 0.00));
  ret->setAlpha(((const Color*)arguments[0])->getAlpha());
  return ret;
}

Value* Color::saturate(const vector<const Value*>& arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double value = ((const NumberValue*)arguments[1])->getValue();

  Color *ret = Color::fromHSL(hsl[0],
                              min(hsl[1] * 100 + value, 100.00),
                              hsl[2] * 100);
  ret->setAlpha(((const Color*)arguments[0])->getAlpha());
  return ret;
}
Value* Color::desaturate(const vector<const Value*>& arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double value = ((const NumberValue*)arguments[1])->getValue();

  Color *ret = Color::fromHSL(hsl[0],
                              max(hsl[1] * 100 - value, 0.00),
                              hsl[2] * 100);
  ret->setAlpha(((const Color*)arguments[0])->getAlpha());
  return ret;
}

Value* Color::fade(const vector<const Value*>& arguments) {
  const Color* c = static_cast<const Color*>(arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  Color* ret = new Color(c->getRed(), c->getGreen(), c->getBlue(), value * .01);
  return ret;
}

Value* Color::fadein(const vector<const Value*>& arguments) {
  const Color* c = static_cast<const Color*>(arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  Color* ret = new Color(
      c->getRed(), c->getGreen(), c->getBlue(), c->getAlpha() + value * .01);
  return ret;
}

Value* Color::fadeout(const vector<const Value*>& arguments) {
  const Color* c = static_cast<const Color*>(arguments[0]);
  double value = ((const NumberValue*)arguments[1])->getValue();

  Color* ret = new Color(
      c->getRed(), c->getGreen(), c->getBlue(), c->getAlpha() - value * .01);
  return ret;
}

Value* Color::spin(const vector<const Value*>& arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();
  double degrees = ((const NumberValue*)arguments[1])->getValue();

  Color *ret = Color::fromHSL(std::floor(hsl[0] + degrees),
                              hsl[1] * 100,
                              hsl[2] * 100);
  ret->setAlpha(((const Color*)arguments[0])->getAlpha());
  return ret;
}

Value* Color::hsl(const vector<const Value*>& arguments) {
  return Color::fromHSL(((const NumberValue*)arguments[0])->getValue(),
                        ((const NumberValue*)arguments[1])->getValue(),
                        ((const NumberValue*)arguments[2])->getValue());
}

Value* Color::hue(const vector<const Value*>& arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();

  return new NumberValue(hsl[0]);
}

Value* Color::saturation(const vector<const Value*>& arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();

  return new NumberValue(hsl[1] * 100, Token::PERCENTAGE, NULL);
}

Value* Color::lightness(const vector<const Value*>& arguments) {
  double* hsl = ((const Color*)arguments[0])->getHSL();

  return new NumberValue(hsl[2] * 100, Token::PERCENTAGE, NULL);
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
  t = Token(hash, Token::STRING, 0, 0, "generated");
  return new StringValue(t, false);
}

Value* Color::red(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getRed());
}
Value* Color::blue(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getBlue());
}
Value* Color::green(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getGreen());
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
