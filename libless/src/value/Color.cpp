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
  float x = chroma * (1 - abs(std::fmod(h, 2.0f) - 1));

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

  convert_hcm_rgb(hsv[HSV_HUE], c, m, rgb);
}

float Color::convert_rgb_hue(const float rgb[3], float chroma,
                             float max) const {
  float hue = 0;
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


Color::Color(bool _hsv, float hue, float saturation, float value, float alpha)
  : Value() {
  (void)_hsv;
  
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
    len = 1;
    break;
  case 4:
    len = 1;
    break;
    
  case 9:
    alpha = 0;
    len = 2;
    break;
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

  switch(blend) {
  case BT_MULTIPLY:

    // base x blend
    for(i = 0; i < 3; i++) {
      rgb1[i] = (unsigned int)(.5 + (float)(rgb1[i] * rgb2[i]) / 0xFF);
    }
    break;
    
  case BT_SCREEN:
    // 1 - (1 - base) x (1 - blend)
    for(i = 0; i < 3; i++) {
      rgb1[i] = 0xFF -
        (unsigned int)((float)((0xFF - rgb1[i]) * (0xFF - rgb2[i]))
                       / 0xFF
                       + .5);
    }
    break;
    
  case BT_OVERLAY:
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
    
  case BT_SOFTLIGHT:
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
    
  case BT_HARDLIGHT:
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
    
  case BT_DIFFERENCE:
    // R = | base - blend |
    for(i = 0; i < 3; i++) {
      rgb1[i] = rgb1[i] < rgb2[i] ? rgb2[i] - rgb1[i] : rgb1[i] - rgb2[i];
    }
    break;
    
  case BT_EXCLUSION:
    // R = 1/2 - 2 x (base - 1/2) x (blend - 1/2)
    for(i = 0; i < 3; i++) {
      rgb1[i] = (127.5 - (2 * ((float)rgb1[i] - 127.5) * ((float)rgb2[i] -
    127.5)) / 256) + .5;
    }

    break;
    
  case BT_AVERAGE:
    // R = ( base + blend ) / 2
    for(i = 0; i < 3; i++) {
      rgb1[i] = ((float)(rgb1[i] + rgb2[i]) * .5 + .5);
    }
    break;
    
  case BT_NEGATION:
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
      cret->increaseRGB(-(int)rgb[RGB_RED], -(int)rgb[RGB_GREEN], -(int)rgb[RGB_BLUE]);
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
      cret = new Color(*this);
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


std::map<string,const char*> create_map_ColorNames()
{
	std::map<string,const char*> m;
	m["black"] =		"#000000";
	m["silver"] =		"#c0c0c0";
	m["gray"] =		"#808080";
	m["white"] =		"#ffffff";
	m["maroon"] = 		"#800000";
	m["red"] = 		"#ff0000";
	m["purple"] = 		"#800080";
	m["fuchsia"] =		"#ff00ff";
	m["green"] = 		"#008000";
	m["lime"] = 		"#00ff00";
	m["olive"] = 		"#808000";
	m["yellow"] = 		"#ffff00";
	m["navy"] = 		"#000080";
	m["blue"] = 		"#0000ff";
	m["teal"] = 		"#008080";
	m["aqua"] = 		"#00ffff";
	m["orange"] = 		"#ffa500";
	m["aliceblue"] = 	"#f0f8ff";
	m["antiquewhite"] = 	"#faebd7";
	m["aquamarine"] = 	"#7fffd4";
	m["azure"] = 		"#f0ffff";
	m["beige"] = 		"#f5f5dc";
	m["bisque"] = 		"#ffe4c4";
	m["blanchedalmond"] = 	"#ffebcd";
	m["blueviolet"] = 	"#8a2be2";
	m["brown"] = 		"#a52a2a";
	m["burlywood"] = 	"#deb887";
	m["cadetblue"] = 	"#5f9ea0";
	m["chartreuse"] = 	"#7fff00";
	m["chocolate"] = 	"#d2691e";
	m["coral"] = 		"#ff7f50";
	m["cornflowerblue"] = 	"#6495ed";
	m["cornsilk"] = 	"#fff8dc";
	m["crimson"] = 	"#dc143c";
	m["cyan"] = 		"#00ffff";
	m["darkblue"] = 	"#00008b";
	m["darkcyan"] = 	"#008b8b";
	m["darkgoldenrod"] = 	"#b8860b";
	m["darkgray"] = 	"#a9a9a9";
	m["darkgreen"] = 	"#006400";
	m["darkgrey"] = 	"#a9a9a9";
	m["darkkhaki"] = 	"#bdb76b";
	m["darkmagenta"] = 	"#8b008b";
	m["darkolivegreen"] = 	"#556b2f";
	m["darkorange"] = 	"#ff8c00";
	m["darkorchid"] = 	"#9932cc";
	m["darkred"] = 	"#8b0000";
	m["darksalmon"] = 	"#e9967a";
	m["darkseagreen"] = 	"#8fbc8f";
	m["darkslateblue"] = 	"#483d8b";
	m["darkslategray"] = 	"#2f4f4f";
	m["darkslategrey"] = 	"#2f4f4f";
	m["darkturquoise"] = 	"#00ced1";
	m["darkviolet"] = 	"#9400d3";
	m["deeppink"] = 	"#ff1493";
	m["deepskyblue"] = 	"#00bfff";
	m["dimgray"] = 	"#696969";
	m["dimgrey"] = 	"#696969";
	m["dodgerblue"] = 	"#1e90ff";
	m["firebrick"] = 	"#b22222";
	m["floralwhite"] = 	"#fffaf0";
	m["forestgreen"] = 	"#228b22";
	m["gainsboro"] = 	"#dcdcdc";
	m["ghostwhite"] = 	"#f8f8ff";
	m["gold"] = 		"#ffd700";
	m["goldenrod"] = 	"#daa520";
	m["greenyellow"] = 	"#adff2f";
	m["grey"] = 		"#808080";
	m["honeydew"] = 	"#f0fff0";
	m["hotpink"] = 	"#ff69b4";
	m["indianred"] = 	"#cd5c5c";
	m["indigo"] = 		"#4b0082";
	m["ivory"] = 		"#fffff0";
	m["khaki"] = 		"#f0e68c";
	m["lavender"] = 	"#e6e6fa";
	m["lavenderblush"] = 	"#fff0f5";
	m["lawngreen"] = 	"#7cfc00";
	m["lemonchiffon"] = 	"#fffacd";
	m["lightblue"] = 	"#add8e6";
	m["lightcoral"] = 	"#f08080";
	m["lightcyan"] = 	"#e0ffff";
	m["lightgoldenrodyellow"] = 	"#fafad2";
	m["lightgray"] = 	"#d3d3d3";
	m["lightgreen"] = 	"#90ee90";
	m["lightgrey"] = 	"#d3d3d3";
	m["lightpink"] = 	"#ffb6c1";
	m["lightsalmon"] = 	"#ffa07a";
	m["lightseagreen"] = 	"#20b2aa";
	m["lightskyblue"] = 	"#87cefa";
	m["lightslategray"] = 	"#778899";
	m["lightslategrey"] = 	"#778899";
	m["lightsteelblue"] = 	"#b0c4de";
	m["lightyellow"] = 	"#ffffe0";
	m["limegreen"] = 	"#32cd32";
	m["linen"] = 		"#faf0e6";
	m["magenta"] = 	"#ff00ff";
	m["mediumaquamarine"] = 	"#66cdaa";
	m["mediumblue"] = 	"#0000cd";
	m["mediumorchid"] = 	"#ba55d3";
	m["mediumpurple"] = 	"#9370db";
	m["mediumseagreen"] = 	"#3cb371";
	m["mediumslateblue"] =	"#7b68ee";
	m["mediumspringgreen"] = 	"#00fa9a";
	m["mediumturquoise"] =	"#48d1cc";
	m["mediumvioletred"] =	"#c71585";
	m["midnightblue"] = 	"#191970";
	m["mintcream"] = 	"#f5fffa";
	m["mistyrose"] = 	"#ffe4e1";
	m["moccasin"] = 	"#ffe4b5";
	m["navajowhite"] = 	"#ffdead";
	m["oldlace"] = 	"#fdf5e6";
	m["olivedrab"] = 	"#6b8e23";
	m["orangered"] = 	"#ff4500";
	m["orchid"] = 		"#da70d6";
	m["palegoldenrod"] = 	"#eee8aa";
	m["palegreen"] = 	"#98fb98";
	m["paleturquoise"] = 	"#afeeee";
	m["palevioletred"] = 	"#db7093";
	m["papayawhip"] = 	"#ffefd5";
	m["peachpuff"] = 	"#ffdab9";
	m["peru"] = 		"#cd853f";
	m["pink"] = 		"#ffc0cb";
	m["plum"] = 		"#dda0dd";
	m["powderblue"] = 	"#b0e0e6";
	m["rosybrown"] = 	"#bc8f8f";
	m["royalblue"] = 	"#4169e1";
	m["saddlebrown"] = 	"#8b4513";
	m["salmon"] = 		"#fa8072";
	m["sandybrown"] = 	"#f4a460";
	m["seagreen"] = 		"#2e8b57";
	m["seashell"] = 		"#fff5ee";
	m["sienna"] = 		"#a0522d";
	m["skyblue"] = 		"#87ceeb";
	m["slateblue"] = 	"#6a5acd";
	m["slategray"] = 	"#708090";
	m["slategrey"] = 	"#708090";
	m["snow"] = 		"#fffafa";
	m["springgreen"] = 	"#00ff7f";
	m["steelblue"] = 	"#4682b4";
	m["tan"] = 		"#d2b48c";
	m["thistle"] = 		"#d8bfd8";
	m["tomato"] = 		"#ff6347";
	m["turquoise"] = 	"#40e0d0";
	m["violet"] = 		"#ee82ee";
	m["wheat"] = 		"#f5deb3";
	m["whitesmoke"] = 	"#f5f5f5";
	m["yellowgreen"] = 	"#9acd32";
	m["rebeccapurple"] = 	"#663399";
	m["transparent"] =	"#00000000";

	return m;
}

std::map<string,const char*> Color::ColorNames = create_map_ColorNames();