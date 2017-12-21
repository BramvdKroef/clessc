#ifndef __less_value_Color_h__
#define __less_value_Color_h__

#include <algorithm>
#include <cmath>
#include "less/value/FunctionLibrary.h"
#include "less/value/NumberValue.h"
#include "less/value/StringValue.h"
#include "less/value/Value.h"

using namespace std;

#define RGB_RED 0
#define RGB_GREEN 1
#define RGB_BLUE 2
//#define ABS(x) (x < 0 ? 0 - x : x)

class Color : public Value {
private:
  unsigned int color[3];
  double alpha;

  double maxArray(double* array, const size_t len) const;
  double minArray(double* array, const size_t len) const;

  void updateTokens();

public:
  Color();
  Color(Token& token);
  Color(unsigned int red, unsigned int green, unsigned int blue);
  Color(unsigned int red, unsigned int green, unsigned int blue, double alpha);
  Color(const Color& color);

  /**
   * The HSL to RGB conversion on
   * http://en.wikipedia.org/wiki/HSL_and_HSV did not work at all so
   * the calculations are from
   * http://130.113.54.154/~monger/hsl-rgb.html, which does not list a
   * source.
   */
  static Color* fromHSL(double hue, double saturation, double lightness);

  virtual ~Color();

  virtual Value* add(const Value& v) const;
  virtual Value* substract(const Value& v) const;
  virtual Value* multiply(const Value& v) const;
  virtual Value* divide(const Value& v) const;

  virtual BooleanValue* equals(const Value& v) const;
  virtual BooleanValue* lessThan(const Value& v) const;

  /**
   * Converts the internal RGB value to HSL. The source of the
   * calculations is http://en.wikipedia.org/wiki/HSL_and_HSV except
   * for the saturation value, which did not work.
   */
  double* getHSL() const;

  /**
   * Change the color to a new rgb value.
   */
  void setRGB(unsigned int red, unsigned int green, unsigned int blue);

  /**
   * Returns the the amount of red in the color.
   *
   * @return an int value between 0-255
   */
  unsigned int getRed() const;
  /**
   * Returns the the amount of green in the color.
   *
   * @return an int value between 0-255
   */
  unsigned int getGreen() const;
  /**
   * Returns the the amount of blue in the color.
   *
   * @return an int value between 0-255
   */
  unsigned int getBlue() const;

  void setAlpha(double alpha);
  double getAlpha() const;

  static void loadFunctions(FunctionLibrary& lib);
  static Value* rgb(const vector<const Value*>& arguments);
  static Value* rgba(const vector<const Value*>& arguments);
  static Value* lighten(const vector<const Value*>& arguments);
  static Value* darken(const vector<const Value*>& arguments);
  static Value* saturate(const vector<const Value*>& arguments);
  static Value* desaturate(const vector<const Value*>& arguments);
  static Value* fadein(const vector<const Value*>& arguments);
  static Value* fadeout(const vector<const Value*>& arguments);
  static Value* spin(const vector<const Value*>& arguments);
  static Value* hsl(const vector<const Value*>& arguments);
  static Value* hue(const vector<const Value*>& arguments);
  static Value* saturation(const vector<const Value*>& arguments);
  static Value* lightness(const vector<const Value*>& arguments);
  static Value* argb(const vector<const Value*>& arguments);
  static Value* red(const vector<const Value*>& arguments);
  static Value* green(const vector<const Value*>& arguments);
  static Value* blue(const vector<const Value*>& arguments);
  static Value* _alpha(const vector<const Value*>& arguments);

  static Value* hsla(const vector<const Value*>& arguments);
  static Value* hsv(const vector<const Value*>& arguments);
  static Value* hsva(const vector<const Value*>& arguments);
  static Value* hsvhue(const vector<const Value*>& arguments);
  static Value* hsvsaturation(const vector<const Value*>& arguments);
  static Value* hsvvalue(const vector<const Value*>& arguments);
  static Value* luma(const vector<const Value*>& arguments);
  static Value* fade(const vector<const Value*>& arguments);
  static Value* mix(const vector<const Value*>& arguments);
  static Value* greyscale(const vector<const Value*>& arguments);
  static Value* contrast(const vector<const Value*>& arguments);
  static Value* screen(const vector<const Value*>& arguments);
  static Value* overlay(const vector<const Value*>& arguments);
  static Value* softlight(const vector<const Value*>& arguments);
  static Value* hardlight(const vector<const Value*>& arguments);
  static Value* difference(const vector<const Value*>& arguments);
  static Value* exclusion(const vector<const Value*>& arguments);
  static Value* avarage(const vector<const Value*>& arguments);
  static Value* negation(const vector<const Value*>& arguments);
};

#endif  // __less_value_Color_h__
