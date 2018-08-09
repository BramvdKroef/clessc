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
#define HSV_HUE 0
#define HSV_SATURATION 1
#define HSV_VALUE 2

//#define ABS(x) (x < 0 ? 0 - x : x)

class FunctionLibrary;

class Color : public Value {
private:
  /**
   * Used for converting hsl/hsv colors to rgb.
   */
  static const int conversion_matrices[6][3][2];

  /**
   * Generate rgb values from hue, chroma and match value.
   *
   * @see convert_hsl_rgb
   * @see convert_hsv_rgb
   */
  void convert_hcm_rgb(float hue, float chroma, float match,
                       unsigned int rgb[3]) const;
  /**
   * Convert hue, saturation and lightness values to red, green and blue using
   * the calculations from https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSL
   *
   * @param hsl   an array containing
   *              - [HSL_HUE]: the hue as a value between 0 and 359.
   *              - [HSL_SATURATION]: the saturation value between 0 and 1.
   *              - [HSL_LIGHTNESS]: the lightness value between 0 and 1.
   * @param rgb   an array that will be assigned the rgb values in the order
   *              red, green and blue.
   */
  void convert_hsl_rgb(const float hsl[3], unsigned int rgb[3]) const; 

  /**
   * Convert hue, saturation and value values to red, green and blue using the
   * calculations from https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
   *
   * @param hsv   an array containing
   *              - [HSV_HUE]: the hue as a value between 0 and 359.
   *              - [HSV_SATURATION]: the saturation value between 0 and 1.
   *              - [HSV_VALUE]: the value value between 0 and 1.
   * @param rgb   an array that will be assigned the rgb values in the order
   *              red, green and blue.
   */
  void convert_hsv_rgb(const float hsv[3], unsigned int rgb[3]) const;

  /**
   * Calculate the hue for rgb values.
   *
   * @param rgb      rgb values in range 0...1
   * @param chroma   calculated from rgb
   * @param max      maximum value from rgb array.
   */
  float convert_rgb_hue(const float rgb[3], float chroma,
                        float max) const;

  /**
   * Converts the internal RGB value to HSL. The source of the
   * calculations is http://en.wikipedia.org/wiki/HSL_and_HSV 
   */
  void convert_rgb_hsl(const unsigned int rgb[3], float hsl[3]) const;

  /**
   * Converts the internal RGB value to HSV. The source of the
   * calculations is http://en.wikipedia.org/wiki/HSL_and_HSV 
   */
  void convert_rgb_hsv(const unsigned int rgb[3], float hsv[3]) const;
  
  unsigned int rgb[3];
  float hsl[3];
  float hsv[3];
  float alpha;

  float maxArray(float* array, const size_t len) const;
  float minArray(float* array, const size_t len) const;

  virtual const TokenList* getTokens() const;

  bool parseHash(const char* hash);
  
public:
  Token token;
  enum ColorType {
    TOKEN,
    RGB,
    HSL,
    HSV
  } color_type;

  enum blendtype {
    BT_MULTIPLY, BT_SCREEN, BT_OVERLAY, BT_SOFTLIGHT, BT_HARDLIGHT, BT_DIFFERENCE, BT_EXCLUSION,
    BT_AVERAGE, BT_NEGATION};
  
  Color(const Token &token);
  Color(const Token &name, const char* hash);
  Color(unsigned int red=0, unsigned int green=0, unsigned int blue=0, float alpha=1.f);
  Color(float hue, float saturation, float lightness, float alpha=1.f);
  Color(bool hsv, float hue, float saturation, float value, float alpha=1.f);
  Color(const Color &color);

  static std::map<string,const char*> ColorNames;

  /**
   * Looks in the ColorNames database for the given name and creates a color
   * object with the rgb value that it represents.
   *
   * @return a new Color object or NULL if the color was not found.
   */
  static Color* fromName(const Token &name);
  
  virtual ~Color();

  virtual Value* operator+(const Value& v) const;
  virtual Value* operator-(const Value& v) const;
  virtual Value* operator*(const Value& v) const;
  virtual Value* operator/(const Value& v) const;

  virtual bool operator==(const Value& v) const;
  virtual bool operator<(const Value& v) const;

  
  void getHSL(float hsl[3]) const;
  void getHSV(float hsl[3]) const;
  void getRGB(unsigned int rgb[3]) const;

  void mix(const Color &color, float weight);

  void blend(const Color &color, blendtype blend);
  
  void setAlpha(float alpha);
  float getAlpha() const;

  float getLuma() const;
  float getLuminance() const;
  /**
   * Increase lightness 
   */
  void lighten(float lightness);
  /**
   * Decrease lightness
   */
  void darken(float lightness);

  /**
   * Increase saturation
   */
  void saturate(float saturation);

  /**
   * Decrease saturation
   */
  void desaturate(float saturation);

  /**
   * Increase hue
   */
  void spin(float hue) ;


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
  void multiplyRGB(float red,
                   float green,
                   float blue);

};

#endif  // __less_value_Color_h__
