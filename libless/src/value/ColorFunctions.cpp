#include "less/value/ColorFunctions.h"
#include "less/value/FunctionLibrary.h"
#include "less/value/Value.h"
#include "less/value/NumberValue.h"
#include "less/value/Color.h"


void ColorFunctions::loadFunctions(FunctionLibrary& lib) {
  lib.push("rgb", "NNN", &ColorFunctions::rgb);
  lib.push("rgba", "NNN.", &ColorFunctions::rgba);
  lib.push("lighten", "CP", &ColorFunctions::lighten);
  lib.push("darken", "CP", &ColorFunctions::darken);
  lib.push("saturate", "CP", &ColorFunctions::saturate);
  lib.push("desaturate", "CP", &ColorFunctions::desaturate);
  lib.push("fade", "CP", &ColorFunctions::fade);
  lib.push("fadein", "CP", &ColorFunctions::fadein);
  lib.push("fadeout", "CP", &ColorFunctions::fadeout);
  lib.push("spin", "CN", &ColorFunctions::spin);
  lib.push("hsl", "NPP", &ColorFunctions::hsl);
  lib.push("hsla", "NPP.", &ColorFunctions::hsla);
  lib.push("hsv", "NPP", &ColorFunctions::hsv);
  lib.push("hsva", "NPP.", &ColorFunctions::hsva);
  lib.push("hue", "C", &ColorFunctions::hue);
  lib.push("saturation", "C", &ColorFunctions::saturation);
  lib.push("lightness", "C", &ColorFunctions::lightness);
  lib.push("hsvhue", "C", &ColorFunctions::hsvhue);
  lib.push("hsvsaturation", "C", &ColorFunctions::hsvsaturation);
  lib.push("hsvvalue", "C", &ColorFunctions::hsvvalue);
  lib.push("argb", "C", &ColorFunctions::argb);
  lib.push("red", "C", &ColorFunctions::red);
  lib.push("blue", "C", &ColorFunctions::blue);
  lib.push("green", "C", &ColorFunctions::green);
  lib.push("alpha", "C", &ColorFunctions::alpha);
  lib.push("luma", "C", &ColorFunctions::luma);
  lib.push("luminance", "C", &ColorFunctions::luminance);
  lib.push("mix", "CCP?", &ColorFunctions::mix);
  lib.push("tint", "CP?", &ColorFunctions::tint);
  lib.push("shade", "CP?", &ColorFunctions::shade);
  lib.push("greyscale", "C", &ColorFunctions::greyscale);
  lib.push("contrast", "CC?C?P?", &ColorFunctions::contrast);
  lib.push("multiply", "CC", &ColorFunctions::multiply);
  lib.push("screen", "CC", &ColorFunctions::screen);
  lib.push("overlay", "CC", &ColorFunctions::overlay);
  lib.push("softlight", "CC", &ColorFunctions::softlight);
  lib.push("hardlight", "CC", &ColorFunctions::hardlight);
  lib.push("difference", "CC", &ColorFunctions::difference);
  lib.push("exclusion", "CC", &ColorFunctions::exclusion);
  lib.push("average", "CC", &ColorFunctions::average);
  lib.push("negation", "CC", &ColorFunctions::negation);
}

Value* ColorFunctions::rgb(const vector<const Value*>& arguments) {
  return new Color(
                   (unsigned int)((const NumberValue*)arguments[0])->getValue(),
                   (unsigned int)((const NumberValue*)arguments[1])->getValue(),
                   (unsigned int)((const NumberValue*)arguments[2])->getValue());
}

Value* ColorFunctions::rgba(const vector<const Value*>& arguments) {
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
Value* ColorFunctions::lighten(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->lighten(value * .01);
  return c;
}
Value* ColorFunctions::darken(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->darken(value * .01);
  return c;
}

Value* ColorFunctions::saturate(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->saturate(value * .01);
  return c;
}
Value* ColorFunctions::desaturate(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->desaturate(value * .01);
  return c;
}

Value* ColorFunctions::fade(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(value * .01);
  return c;
}

Value* ColorFunctions::fadein(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(c->getAlpha() + (value * .01));
  return c;
}

Value* ColorFunctions::fadeout(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float value = ((const NumberValue*)arguments[1])->getValue();

  c->setAlpha(c->getAlpha() - (value * .01));
  return c;
}

Value* ColorFunctions::spin(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float degrees = ((const NumberValue*)arguments[1])->getValue();

  c->spin(degrees);
  return c;
}

Value* ColorFunctions::hsl(const vector<const Value*>& arguments) {
  return new Color((float)((const NumberValue*)arguments[0])->getValue(),
                   (float)(((const NumberValue*)arguments[1])->getValue() * .01),
                   (float)(((const NumberValue*)arguments[2])->getValue() * .01));
}

Value* ColorFunctions::hsla(const vector<const Value*>& arguments) {
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

Value* ColorFunctions::hsv(const vector<const Value*>& arguments) {
  return new Color(true,
                   (float)((const NumberValue*)arguments[0])->getValue(),
                   (float)(((const NumberValue*)arguments[1])->getValue() * .01),
                   (float)(((const NumberValue*)arguments[2])->getValue() * .01));
}

Value* ColorFunctions::hsva(const vector<const Value*>& arguments) {
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

Value* ColorFunctions::hue(const vector<const Value*>& arguments) {
  float hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_HUE]);
}

Value* ColorFunctions::saturation(const vector<const Value*>& arguments) {
  float hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_SATURATION] * 100, Token::PERCENTAGE, NULL);
}

Value* ColorFunctions::lightness(const vector<const Value*>& arguments) {
  float hsl[3];
  ((const Color*)arguments[0])->getHSL(hsl);

  return new NumberValue(hsl[HSL_LIGHTNESS] * 100, Token::PERCENTAGE, NULL);
}

Value* ColorFunctions::hsvhue(const vector<const Value*>& arguments) {
  float hsv[3];
  ((const Color*)arguments[0])->getHSV(hsv);

  return new NumberValue(hsv[HSV_HUE]);
}

Value* ColorFunctions::hsvsaturation(const vector<const Value*>& arguments) {
  float hsv[3];
  ((const Color*)arguments[0])->getHSV(hsv);

  return new NumberValue(hsv[HSV_SATURATION] * 100, Token::PERCENTAGE, NULL);
}

Value* ColorFunctions::hsvvalue(const vector<const Value*>& arguments) {
  float hsv[3];
  ((const Color*)arguments[0])->getHSV(hsv);

  return new NumberValue(hsv[HSV_VALUE] * 100, Token::PERCENTAGE, NULL);
}

Value* ColorFunctions::argb(const vector<const Value*>& arguments) {
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

Value* ColorFunctions::red(const vector<const Value*>& arguments) {
  unsigned int rgb[3];
  ((const Color*)arguments[0])->getRGB(rgb);
  
  return new NumberValue(rgb[RGB_RED]);
}
Value* ColorFunctions::blue(const vector<const Value*>& arguments) {
  unsigned int rgb[3];
  ((const Color*)arguments[0])->getRGB(rgb);
  
  return new NumberValue(rgb[RGB_BLUE]);
}
Value* ColorFunctions::green(const vector<const Value*>& arguments) {
  unsigned int rgb[3];
  ((const Color*)arguments[0])->getRGB(rgb);
  
  return new NumberValue(rgb[RGB_GREEN]);
}
Value* ColorFunctions::alpha(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getAlpha());
}
Value* ColorFunctions::luma(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getLuma() * 100, Token::PERCENTAGE, NULL);
}

Value* ColorFunctions::luminance(const vector<const Value*>& arguments) {
  const Color* c = (const Color*)arguments[0];

  return new NumberValue(c->getLuminance() * 100, Token::PERCENTAGE, NULL);
}

Value* ColorFunctions::mix(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  float weight = .5;
  if (arguments.size() > 2)
    weight = (float)(((const NumberValue*)arguments[2])->getValue() * .01);

  c->mix(*(const Color*)arguments[1], weight);
  return c;
}

Value* ColorFunctions::tint(const vector<const Value*>& arguments) {
  Color *c = new Color((unsigned int)255,(unsigned int)255,(unsigned int)255);
  float weight = .5;
  if (arguments.size() > 1)
    weight = (float)(((const NumberValue*)arguments[1])->getValue() * .01);

  c->mix(*(const Color*)arguments[0], weight);
  return c;
}

Value* ColorFunctions::shade(const vector<const Value*>& arguments) {
  Color *c = new Color((unsigned int)0,(unsigned int)0,(unsigned int)0);
  float weight = .5;
  if (arguments.size() > 1)
    weight = (float)(((const NumberValue*)arguments[1])->getValue() * .01);

  c->mix(*(const Color*)arguments[0], weight);
  return c;
}

Value* ColorFunctions::greyscale(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->desaturate(1);
  return c;
}

Value* ColorFunctions::contrast(const vector<const Value*>& arguments) {
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

Value* ColorFunctions::multiply(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::MULTIPLY);
  return c;
}

Value* ColorFunctions::screen(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::SCREEN);
  return c;
}

Value* ColorFunctions::overlay(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::OVERLAY);
  return c;
}

Value* ColorFunctions::softlight(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::SOFTLIGHT);
  return c;
}

Value* ColorFunctions::hardlight(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::HARDLIGHT);
  return c;
}

Value* ColorFunctions::difference(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::DIFFERENCE);
  return c;
}

Value* ColorFunctions::exclusion(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::EXCLUSION);
  return c;
}

Value* ColorFunctions::average(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::AVERAGE);
  return c;
}

Value* ColorFunctions::negation(const vector<const Value*>& arguments) {
  Color *c = new Color(*(const Color*)arguments[0]);
  c->blend(*(const Color*)arguments[1], Color::NEGATION);
  return c;
}
