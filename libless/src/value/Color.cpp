#include <iostream>
#include <sstream>

#include "less/value/Color.h"

#include "less/value/FunctionLibrary.h"

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

float Color::maxArray(float* array, const size_t len) const {
  float ret = array[0];
  for (size_t i = 1; i < len; i++)
    ret = max(ret, array[i]);
  return ret;
}
float Color::minArray(float* array, const size_t len) const {
  float ret = array[0];
  for (size_t i = 1; i < len; i++)
    ret = min(ret, array[i]);
  return ret;
}


const int Color::conversion_matrices[6][3][2] =
  { //c,x
    {{1,0},  // r
     {0,1},  // g
     {0,0}}, // b
    {{0,1},
     {1,0},
     {0,0}},
    {{0,0},
     {1,0},
     {0,1}},
    {{0,0},
     {0,1},
     {1,0}},
    {{0,1},
     {0,0},
     {1,0}},
    {{1,0},
     {0,0},
     {0,1}}};

void Color::convert_hcm_rgb(float hue, float chroma, float match,
                            unsigned int rgb[3]) const {
  float h = hue / 60;
  float x = chroma * (1 - abs(std::fmod(h, 2.0) - 1));

  const int (* matrix)[2] = conversion_matrices[(int)h];
  
  for (int i = 0; i < 3; i++) {
    rgb[i] = (unsigned int)((matrix[i][0] * chroma +
                             matrix[i][1] * x +
                             match) * 0xFF + .5) & 0xFF;
  }
}
void Color::convert_hsl_rgb(const float hsl[3], unsigned int rgb[3]) const {
  float c = (1.0 - abs(2.0 * hsl[HSL_LIGHTNESS] - 1))
    * hsl[HSL_SATURATION];
  float m = hsl[HSL_LIGHTNESS] - .5 * c;

  convert_hcm_rgb(hsl[HSL_HUE], c, m, rgb);

}

void Color::convert_hsv_rgb(const float hsv[3], unsigned int rgb[3]) const {
  float c = hsv[HSV_VALUE] * hsv[HSV_SATURATION];
  float m = hsv[HSV_VALUE] - c;

  convert_hcm_rgb(hsl[HSL_HUE], c, m, rgb);
}

float Color::convert_rgb_hue(const float rgb[3], float chroma,
                             float max) const {
  float hue;
  if (chroma == 0) {
    hue = 0;
    
  } else if (max == rgb[RGB_RED]) {
    hue = std::fmod((rgb[RGB_GREEN] - rgb[RGB_BLUE]) / chroma, 6);
    
  } else if (max == rgb[RGB_GREEN]) {
    hue = (rgb[RGB_BLUE] - rgb[RGB_RED]) / chroma + 2.0;
  
  } else if (max == rgb[RGB_BLUE])
    hue = (rgb[RGB_RED] - rgb[RGB_GREEN]) / chroma + 4.0;

  hue = 60 * hue;
  return hue;
}

void Color::convert_rgb_hsl(const unsigned int rgb[3], float hsl[3]) const {
  float max, min, c;
  float _rgb[3];

  for (int i = 0; i < 3; i++)
    _rgb[i] = (float)rgb[i] / 0xFF;

  max = maxArray(_rgb, 3);
  min = minArray(_rgb, 3);
  c = max - min;

  hsl[HSL_HUE] = convert_rgb_hue(_rgb, c, max);
  
  hsl[HSL_LIGHTNESS] = (max + min) * .5;

  if (hsl[HSL_LIGHTNESS] == 1)
    hsl[HSL_SATURATION] = 0;
  else
    hsl[HSL_SATURATION] = c / (1.0 - abs(2.0 * hsl[HSL_LIGHTNESS] - 1.0));
}

void Color::convert_rgb_hsv(const unsigned int rgb[3], float hsv[3]) const {
  float max, min, c;
  float _rgb[3];

  for (int i = 0; i < 3; i++)
    _rgb[i] = (float)rgb[i] / 0xFF;

  max = maxArray(_rgb, 3);
  min = minArray(_rgb, 3);
  c = max - min;

  hsv[HSV_HUE] = convert_rgb_hue(_rgb, c, max);
  
  hsv[HSV_VALUE] = max;

  if (hsv[HSV_VALUE] == 0)
    hsv[HSV_SATURATION] = 0;
  else
    hsv[HSV_SATURATION] = c / hsv[HSV_VALUE];
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
             float alpha)
  : Value() {
  type = COLOR;
  color_type = RGB;
  rgb[RGB_RED] = red;
  rgb[RGB_GREEN] = green;
  rgb[RGB_BLUE] = blue;
  this->alpha = alpha;
}

Color::Color(float hue, float saturation, float lightness)
  : Color(hue, saturation, lightness, 1) {
}

Color::Color(float hue, float saturation, float lightness, float alpha)
  : Value() {

  type = COLOR;
  color_type = HSL;
  hsl[HSL_HUE] = std::fmod(hue, 360);
  if (hsl[HSL_HUE] < 0)
    hsl[HSL_HUE] += 360;
  
  hsl[HSL_SATURATION] = max(min(saturation, 1), 0);
  hsl[HSL_LIGHTNESS] = max(min(lightness, 1), 0);
  this->alpha = alpha;
  convert_hsl_rgb(hsl, rgb);
}

Color::Color(bool _hsv, float hue, float saturation, float value)
  : Color(_hsv, hue, saturation, value, 1) {
}

Color::Color(bool _hsv, float hue, float saturation, float value, float alpha)
  : Value() {

  type = COLOR;
  color_type = HSV;
  hsv[HSV_HUE] = std::fmod(hue, 360);
  if (hsv[HSV_HUE] < 0)
    hsv[HSV_HUE] += 360;
  
  hsv[HSV_SATURATION] = max(min(saturation, 1), 0);
  hsv[HSV_VALUE] = max(min(value, 1), 0);
  this->alpha = alpha;
  convert_hsv_rgb(hsv, rgb);
}

Color::Color(const Color &color) : Value() {
  type = COLOR;
  color_type = color.color_type;

  switch (color.color_type) {
  case TOKEN:
    token = color.token;
    tokens.push_back(token);
    break;
    
  case RGB:
    break;
    
  case HSL:
    color.getHSL(hsl);
    break;
    
  case HSV:
    color.getHSV(hsv);
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



void Color::getRGB(unsigned int rgb[3]) const {
  rgb[RGB_RED] = this->rgb[RGB_RED];
  rgb[RGB_GREEN] = this->rgb[RGB_GREEN];
  rgb[RGB_BLUE] = this->rgb[RGB_BLUE];
}

void Color::getHSL(float hsl[3]) const {
  if (color_type == HSL) {

    hsl[HSL_HUE] = this->hsl[HSL_HUE];
    hsl[HSL_SATURATION] = this->hsl[HSL_SATURATION];
    hsl[HSL_LIGHTNESS] = this->hsl[HSL_LIGHTNESS];
  } else {
    convert_rgb_hsl(rgb, hsl);
  }
}

void Color::getHSV(float hsv[3]) const {
  if (color_type == HSV) {

    hsv[HSV_HUE] = this->hsv[HSV_HUE];
    hsv[HSV_SATURATION] = this->hsv[HSV_SATURATION];
    hsv[HSV_VALUE] = this->hsv[HSV_VALUE];
  } else {
    convert_rgb_hsv(rgb, hsv);
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
void Color::multiplyRGB(float red,
                        float green,
                        float blue) {
  if (color_type != RGB) 
    color_type = RGB;
  
  rgb[RGB_RED] = min(rgb[RGB_RED] * red, 255);
  rgb[RGB_GREEN] = min(rgb[RGB_GREEN] * green, 255);
  rgb[RGB_BLUE] = min(rgb[RGB_BLUE] * blue, 255);
}

void Color::lighten(float lightness) {
  if (color_type != HSL) {
    convert_rgb_hsl(rgb, hsl);
    color_type = HSL;
  }
  hsl[HSL_LIGHTNESS] = max(min(hsl[HSL_LIGHTNESS] + lightness, 1), 0);
  convert_hsl_rgb(hsl, rgb);
}

void Color::darken(float lightness) {
  lighten(-lightness);
}

void Color::saturate(float saturation) {
  if (color_type != HSL) {
    convert_rgb_hsl(rgb, hsl);
    color_type = HSL;
  }

  hsl[HSL_SATURATION] = max(min(hsl[HSL_SATURATION] + saturation, 1), 0);

  convert_hsl_rgb(hsl, rgb);
}

void Color::desaturate(float saturation) {
  saturate(-saturation);
}

void Color::spin(float hue) {
  if (color_type != HSL) {
    convert_rgb_hsl(rgb, hsl);
    color_type = HSL;
  }
  hsl[HSL_HUE] = std::fmod(hsl[HSL_HUE] + hue, 360);
  if (hsl[HSL_HUE] < 0)
    hsl[HSL_HUE] += 360;
  
  convert_hsl_rgb(hsl, rgb);
}

void Color::mix(const Color &color, float weight) {
  unsigned int rgb1[3], rgb2[3];
  getRGB(rgb1);
  color.getRGB(rgb2);

  // move weight by the relative alpha.
  float cweight = weight + (color.getAlpha() - getAlpha()) * .5;
  
  for (int i = 0; i < 3; i++) {
    // move color by difference x weight ( and round ).
    rgb1[i] = (int)rgb1[i] + ((int)rgb2[i] - (int)rgb1[i]) * cweight + .5;
  }
  setRGB(rgb1[0], rgb1[1], rgb1[2]);

  setAlpha(getAlpha() + (color.getAlpha() - getAlpha()) * weight );
}

void Color::blend(const Color &color, blendtype blend) {
  unsigned int rgb1[3], rgb2[3];
  getRGB(rgb1);
  color.getRGB(rgb2);
  int i;
  float tmp;

  switch(blend) {
  case MULTIPLY:

    // base x blend
    for(i = 0; i < 3; i++) {
      rgb1[i] = (unsigned int)(.5 + (float)(rgb1[i] * rgb2[i]) / 0xFF);
    }
    break;
    
  case SCREEN:
    // 1 - (1 - base) x (1 - blend)
    for(i = 0; i < 3; i++) {
      rgb1[i] = 0xFF -
        (unsigned int)((float)((0xFF - rgb1[i]) * (0xFF - rgb2[i]))
                       / 0xFF
                       + .5);
    }
    break;
    
  case OVERLAY:
    // 2 x base x blend                  if base < 1/2
    // 1 - 2 x (1 - base) x (1 - blend)  otherwise
    
    for(i = 0; i < 3; i++) {
      if (rgb1[i] < 128)
        rgb1[i] = (unsigned int)(.5 + (float)(2 * rgb1[i] * rgb2[i]) / 0xFF);
      else 
        rgb1[i] = 0xFF -
          (unsigned int)((float)(2 * (0xFF - rgb1[i]) * (0xFF - rgb2[i]))
                         / 0xFF
                         + .5);
    }
    break;
    
  case SOFTLIGHT:
    // TODO: doesn't work yet.
    // base x blend                  if base < 1/2
    // 1 - 2 x (1 - base) x (1 - blend)  otherwise
    
    for(i = 0; i < 3; i++) {
      if (rgb2[i] < 128)
        rgb1[i] = (unsigned int)(.5 + (float)((int)rgb1[i] * ((int)rgb2[i] - 128)) / 0xFF);
      else 
        rgb1[i] = 0xFF -
          (unsigned int)((float)((0xFF - rgb1[i]) * (int)(0xFF - rgb2[i] - 128))
                         / 0xFF
                         + .5);
    }
    
    break;
    
  case HARDLIGHT:
    // 2 x base x blend                  if blend < 1/2
    // 1 - 2 x (1 - base) x (1 - blend)  otherwise
    
    for(i = 0; i < 3; i++) {
      if (rgb2[i] < 128)
        rgb1[i] = (unsigned int)(.5 + (float)(2 * rgb1[i] * rgb2[i]) / 0xFF);
      else 
        rgb1[i] = 0xFF -
          (unsigned int)((float)(2 * (0xFF - rgb1[i]) * (0xFF - rgb2[i]))
                         / 0xFF
                         + .5);
    }
    break;
    
  case DIFFERENCE:
    // R = | base - blend |
    for(i = 0; i < 3; i++) {
      rgb1[i] = rgb1[i] < rgb2[i] ? rgb2[i] - rgb1[i] : rgb1[i] - rgb2[i];
    }
    break;
    
  case EXCLUSION:
    // R = 1/2 - 2 x (base - 1/2) x (blend - 1/2)
    for(i = 0; i < 3; i++) {
      rgb1[i] = (127.5 - (2 * ((float)rgb1[i] - 127.5) * ((float)rgb2[i] -
    127.5)) / 256) + .5;
    }

    break;
    
  case AVERAGE:
    // R = ( base + blend ) / 2
    for(i = 0; i < 3; i++) {
      rgb1[i] = ((float)(rgb1[i] + rgb2[i]) * .5 + .5);
    }
    break;
    
  case NEGATION:
    // TODO: Doesn't work; no documentation found.
    for(i = 0; i < 3; i++) {
      rgb1[i] = rgb1[i] < rgb2[i] ? rgb2[i] - rgb1[i] : rgb1[i] - rgb2[i];
    }

    break;
  }
  setRGB(rgb1[0], rgb1[1], rgb1[2]);
  
}

void Color::setAlpha(float alpha) {
  this->alpha = alpha;
  if (color_type == TOKEN)
    color_type = RGB;
}
float Color::getAlpha() const {
  return alpha;
}

float Color::getLuma() const {
  unsigned int rgb[3];
  float _rgb[3];
  getRGB(rgb);
  for (int i = 0; i < 3; i++) {
    _rgb[i] = ((float)rgb[i] / 0xFF);
    if (_rgb[i] <= 0.03928) {
      _rgb[i] = _rgb[i] / 12.92;
    } else {
      _rgb[i] = pow(((_rgb[i] + 0.055) / 1.055), 2.4);
    }
  }
  return 0.2126 * _rgb[0] + 0.7152 * _rgb[1] + 0.0722 * _rgb[2];
}

float Color::getLuminance() const {
  unsigned int rgb[3];
  float _rgb[3];
  getRGB(rgb);
  for (int i = 0; i < 3; i++) {
    _rgb[i] = ((float)rgb[i] / 0xFF);
  }
  return 0.2126 * _rgb[0] + 0.7152 * _rgb[1] + 0.0722 * _rgb[2];
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


Value* Color::operator+(const Value& v) const {
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
Value* Color::operator-(const Value& v) const {
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

Value* Color::operator*(const Value& v) const {
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
Value* Color::operator/(const Value& v) const {
  const Color* c;
  const NumberValue* n;
  unsigned int rgb[3];
  Color *cret;

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      cret = new Color(*this);
      cret->multiplyRGB((float)1 / rgb[RGB_RED],
                        (float)1 / rgb[RGB_GREEN],
                        (float)1 / rgb[RGB_BLUE]);
      return cret;
    case NUMBER:
    case PERCENTAGE:
    case DIMENSION:
      n = static_cast<const NumberValue*>(&v);
      cret->multiplyRGB((float)1 / n->getValue(),
                        (float)1 / n->getValue(),
                        (float)1 / n->getValue());
      return cret;
    default:
      throw new ValueException(
          "You can only divide a color by a \
color or a number.",
          *this->getTokens());
  }
}

bool Color::operator==(const Value& v) const {
  const Color* c;
  const BooleanValue* b;
  unsigned int rgb[3];

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      
      return (this->rgb[RGB_RED] == rgb[RGB_RED] &&
              this->rgb[RGB_GREEN] == rgb[RGB_GREEN] &&
              this->rgb[RGB_BLUE] == rgb[RGB_BLUE]);

  case BOOLEAN:
    // any color is falsy.
    b = static_cast<const BooleanValue*>(&v);
    return false == b->getValue();
    
  default:
      throw new ValueException("You can only compare a color with a *color*.",
                               *this->getTokens());
  }
}

bool Color::operator<(const Value& v) const {
  const Color* c;
  const BooleanValue* b;
  unsigned int rgb[3];

  switch (v.type) {
    case COLOR:
      c = static_cast<const Color*>(&v);
      c->getRGB(rgb);
      return (this->rgb[RGB_RED] < rgb[RGB_RED] ||
              this->rgb[RGB_GREEN] < rgb[RGB_GREEN] ||
              this->rgb[RGB_BLUE] < rgb[RGB_BLUE]);
  case BOOLEAN:
    b = static_cast<const BooleanValue*>(&v);
    return b->getValue();

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
  lib.push("hsv", "NPP", &Color::_hsv);
  lib.push("hsva", "NPP.", &Color::hsva);
  lib.push("hue", "C", &Color::hue);
  lib.push("saturation", "C", &Color::saturation);
  lib.push("lightness", "C", &Color::lightness);
  lib.push("hsvhue", "C", &Color::hsvhue);
  lib.push("hsvsaturation", "C", &Color::hsvsaturation);
  lib.push("hsvvalue", "C", &Color::hsvvalue);
  lib.push("argb", "C", &Color::argb);
  lib.push("red", "C", &Color::red);
  lib.push("blue", "C", &Color::blue);
  lib.push("green", "C", &Color::green);
  lib.push("alpha", "C", &Color::_alpha);
  lib.push("luma", "C", &Color::luma);
  lib.push("luminance", "C", &Color::luminance);
  lib.push("mix", "CCP?", &Color::_mix);
  lib.push("tint", "CP?", &Color::tint);
  lib.push("shade", "CP?", &Color::shade);
  lib.push("greyscale", "C", &Color::greyscale);
  lib.push("contrast", "CC?C?P?", &Color::contrast);
  lib.push("multiply", "CC", &Color::_multiply);
  lib.push("screen", "CC", &Color::screen);
  lib.push("overlay", "CC", &Color::overlay);
  lib.push("softlight", "CC", &Color::softlight);
  lib.push("hardlight", "CC", &Color::hardlight);
  lib.push("difference", "CC", &Color::difference);
  lib.push("exclusion", "CC", &Color::exclusion);
  lib.push("average", "CC", &Color::average);
  lib.push("negation", "CC", &Color::negation);
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
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->lighten(value * .01);
  return c;
}
Value* Color::_darken(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->darken(value * .01);
  return c;
}

Value* Color::_saturate(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->saturate(value * .01);
  return c;
}
Value* Color::_desaturate(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->desaturate(value * .01);
  return c;
}

Value* Color::fade(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(value * .01);
  return c;
}

Value* Color::fadein(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(c->getAlpha() + (value * .01));
  return c;
}

Value* Color::fadeout(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(c->getAlpha() - (value * .01));
  return c;
}

Value* Color::spin(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float degrees = ((const NumberValue*)arguments[1])->getValue();

  c->spin(degrees);
  return c;
}

Value* Color::_hsl(const vector<const Value*>& arguments) {
  return new Color((float)((const NumberValue*)arguments[0])->getValue(),
                   (float)(((const NumberValue*)arguments[1])->getValue() * .01),
                   (float)(((const NumberValue*)arguments[2])->getValue() * .01));
}

Value* Color::hsla(const vector<const Value*>& arguments) {
  float alpha;
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
  return new Color((float)((const NumberValue*)arguments[0])->getValue(),
                   (float)(((const NumberValue*)arguments[1])->getValue() * .01),
                   (float)(((const NumberValue*)arguments[2])->getValue() * .01),
                   alpha);
}

Value* Color::_hsv(const vector<const Value*>& arguments) {
  return new Color(true,
                   (float)((const NumberValue*)arguments[0])->getValue(),
                   (float)(((const NumberValue*)arguments[1])->getValue() * .01),
                   (float)(((const NumberValue*)arguments[2])->getValue() * .01));
}

Value* Color::hsva(const vector<const Value*>& arguments) {
  float alpha;
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
  return new Color(true,
                   (float)((const NumberValue*)arguments[0])->getValue(),
                   (float)(((const NumberValue*)arguments[1])->getValue() * .01),
                   (float)(((const NumberValue*)arguments[2])->getValue() * .01),
                   alpha);
}

Value* Color::hue(const vector<const Value*>& arguments) {
  float hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_HUE]);
}

Value* Color::saturation(const vector<const Value*>& arguments) {
  float hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_SATURATION] * 100, Token::PERCENTAGE, NULL);
}

Value* Color::lightness(const vector<const Value*>& arguments) {
  float hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_LIGHTNESS] * 100, Token::PERCENTAGE, NULL);
}

Value* Color::hsvhue(const vector<const Value*>& arguments) {
  float hsv[3];
  ((const Color*)arguments[0])->getHSV(hsv);

  return new NumberValue(hsv[HSV_HUE]);
}

Value* Color::hsvsaturation(const vector<const Value*>& arguments) {
  float hsv[3];
  ((const Color*)arguments[0])->getHSV(hsv);

  return new NumberValue(hsv[HSV_SATURATION] * 100, Token::PERCENTAGE, NULL);
}

Value* Color::hsvvalue(const vector<const Value*>& arguments) {
  float hsv[3];
  ((const Color*)arguments[0])->getHSV(hsv);

  return new NumberValue(hsv[HSV_VALUE] * 100, Token::PERCENTAGE, NULL);
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
Value* Color::luma(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getLuma() * 100, Token::PERCENTAGE, NULL);
}

Value* Color::luminance(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getLuminance() * 100, Token::PERCENTAGE, NULL);
}

Value* Color::_mix(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float weight = .5;
  if (arguments.size() > 2)
    weight = (float)(((const NumberValue*)arguments[2])->getValue() * .01);

  c->mix(*(const Color*)arguments[1], weight);
  return c;
}
Value* Color::tint(const vector<const Value*>& arguments) {
  Color *c = new Color((unsigned int)255,(unsigned int)255,(unsigned int)255);
 float weight = .5;
  if (arguments.size() > 1)
    weight = (float)(((const NumberValue*)arguments[1])->getValue() * .01);

  c->mix(*(const Color*)arguments[0], weight);
  return c;
}

Value* Color::shade(const vector<const Value*>& arguments) {
  Color *c = new Color((unsigned int)0,(unsigned int)0,(unsigned int)0);
  float weight = .5;
  if (arguments.size() > 1)
    weight = (float)(((const NumberValue*)arguments[1])->getValue() * .01);

  c->mix(*(const Color*)arguments[0], weight);
  return c;
}

Value* Color::greyscale(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->desaturate(1);
  return c;
}

Value* Color::contrast(const vector<const Value*>& arguments) {
  const Color* compare = (const Color*)arguments[0];
  const Color *dark, *light;
  double threshold = 0.43;
  Color black((unsigned int)0, (unsigned int)0, (unsigned int)0);
  Color white((unsigned int)255, (unsigned int)255, (unsigned int)255);
  float c_luma, d_luma, l_luma;
  
  if (arguments.size() > 1) 
    dark = (const Color*)arguments[1];
  else 
    dark = &black;

  if (arguments.size() > 2) 
    light = (const Color*)arguments[2];
  else 
    light = &white;

  if (arguments.size() > 3)
    threshold = ((const NumberValue*)arguments[3])->getValue() * .01;
  
  c_luma = compare->getLuma();
  d_luma = dark->getLuma();
  l_luma = light->getLuma();

  if (d_luma < l_luma) {
    if (c_luma < d_luma + (l_luma - d_luma) * threshold) 
      return new Color(*light);
    else
      return new Color(*dark);
  } else {
    if (c_luma < l_luma + (d_luma - l_luma) * threshold) 
      return new Color(*dark);
    else
      return new Color(*light);
  }
}

Value* Color::_multiply(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], MULTIPLY);
  return c;
}

Value* Color::screen(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], SCREEN);
  return c;
}

Value* Color::overlay(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], OVERLAY);
  return c;
}

Value* Color::softlight(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], SOFTLIGHT);
  return c;
}

Value* Color::hardlight(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], HARDLIGHT);
  return c;
}

Value* Color::difference(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], DIFFERENCE);
  return c;
}

Value* Color::exclusion(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], EXCLUSION);
  return c;
}

Value* Color::average(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], AVERAGE);
  return c;
}

Value* Color::negation(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], NEGATION);
  return c;
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
