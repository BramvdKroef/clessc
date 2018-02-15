#ifndef __less_value_Color_h__
#define __less_value_Color_h__

#include <algorithm>
#include <cmath>
#include <map>
#include "less/value/NumberValue.h"
#include "less/value/StringValue.h"
#include "less/value/Value.h"

using namespace std;

#define RGB_RED 0
#define RGB_GREEN 1
#define RGB_BLUE 2
#define HSL_HUE 0
#define HSL_SATURATION 1
#define HSL_LIGHTNESS 2

//#define ABS(x) (x < 0 ? 0 - x : x)

class FunctionLibrary;

class Color : public Value {
private:
  unsigned int rgb[3];
  double hsl[3];
  double alpha;

  double maxArray(double* array, const size_t len) const;
  double minArray(double* array, const size_t len) const;

  virtual const TokenList* getTokens() const;

  bool parseHash(const char* hash);
  
public:
  Token token;
  enum ColorType {
    TOKEN,
    RGB,
    HSL
  } color_type;
  
  Color();
  Color(const Token &token);
  Color(const Token &name, const char* hash);
  Color(unsigned int red, unsigned int green, unsigned int blue);
  Color(unsigned int red, unsigned int green, unsigned int blue, double alpha);
  Color(double hue, double saturation, double lightness);
  Color(double hue, double saturation, double lightness, double alpha);
  Color(const Color &color);

  static std::map<string,const char*> ColorNames;

  /**
   * Looks in the ColorNames database for the given name and creates a color
   * object with the rgb value that it represents.
   *
   * @return a new Color object or NULL if the color was not found.
   */
  static Color* fromName(const Token &name);
  
  /**
   * The HSL to RGB conversion on
   * http://en.wikipedia.org/wiki/HSL_and_HSV did not work at all so
   * the calculations are from
   * http://130.113.54.154/~monger/hsl-rgb.html, which does not list a
   * source.
   */
  void HSLtoRGB();

  /**
   * Converts the internal RGB value to HSL. The source of the
   * calculations is http://en.wikipedia.org/wiki/HSL_and_HSV except
   * for the saturation value, which did not work.
   */
  void RGBtoHSL(double hsl[3]) const;

  virtual ~Color();

  virtual Value* add(const Value& v) const;
  virtual Value* substract(const Value& v) const;
  virtual Value* multiply(const Value& v) const;
  virtual Value* divide(const Value& v) const;

  virtual BooleanValue* equals(const Value& v) const;
  virtual BooleanValue* lessThan(const Value& v) const;

  
  void getHSL(double hsl[3]) const;
  void getRGB(unsigned int rgb[3]) const;

  void setAlpha(double alpha);
  double getAlpha() const;

  /**
   * Increase lightness 
   */
  void lighten(double lightness);
  /**
   * Decrease lightness
   */
  void darken(double lightness);

  /**
   * Increase saturation
   */
  void saturate(double saturation);

  /**
   * Decrease saturation
   */
  void desaturate(double saturation);

  /**
   * Increase hue
   */
  void spin(double hue) ;


  /**
   * Set rgb values
   */
  void setRGB(unsigned int red, unsigned int green, unsigned int blue);
    
  /**
   * Increase rgb values
   */
  void increaseRGB(int red,
                   int green,
                   int blue);
  /**
   * Multiply rgb values
   */
  void multiplyRGB(double red,
                   double green,
                   double blue);

  static void loadFunctions(FunctionLibrary& lib);
  static Value* _rgb(const vector<const Value*>& arguments);
  static Value* rgba(const vector<const Value*>& arguments);
  static Value* _lighten(const vector<const Value*>& arguments);
  static Value* _darken(const vector<const Value*>& arguments);
  static Value* _saturate(const vector<const Value*>& arguments);
  static Value* _desaturate(const vector<const Value*>& arguments);
  static Value* fadein(const vector<const Value*>& arguments);
  static Value* fadeout(const vector<const Value*>& arguments);
  static Value* spin(const vector<const Value*>& arguments);
  static Value* _hsl(const vector<const Value*>& arguments);
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
