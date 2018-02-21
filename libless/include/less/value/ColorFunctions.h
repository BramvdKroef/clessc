#ifndef __less_value_ColorFunctions_h__
#define __less_value_ColorFunctions_h__

#include <vector>

class Value;
class FunctionLibrary;

class ColorFunctions {
public:
  static void loadFunctions(FunctionLibrary& lib);
  static Value* rgb(const std::vector<const Value*>& arguments);
  static Value* rgba(const std::vector<const Value*>& arguments);
  static Value* lighten(const std::vector<const Value*>& arguments);
  static Value* darken(const std::vector<const Value*>& arguments);
  static Value* saturate(const std::vector<const Value*>& arguments);
  static Value* desaturate(const std::vector<const Value*>& arguments);
  static Value* fadein(const std::vector<const Value*>& arguments);
  static Value* fadeout(const std::vector<const Value*>& arguments);
  static Value* spin(const std::vector<const Value*>& arguments);
  static Value* hsl(const std::vector<const Value*>& arguments);
  static Value* hue(const std::vector<const Value*>& arguments);
  static Value* saturation(const std::vector<const Value*>& arguments);
  static Value* lightness(const std::vector<const Value*>& arguments);
  static Value* argb(const std::vector<const Value*>& arguments);
  static Value* red(const std::vector<const Value*>& arguments);
  static Value* green(const std::vector<const Value*>& arguments);
  static Value* blue(const std::vector<const Value*>& arguments);
  static Value* alpha(const std::vector<const Value*>& arguments);
  static Value* hsla(const std::vector<const Value*>& arguments);
  static Value* hsv(const std::vector<const Value*>& arguments);
  static Value* hsva(const std::vector<const Value*>& arguments);
  static Value* hsvhue(const std::vector<const Value*>& arguments);
  static Value* hsvsaturation(const std::vector<const Value*>& arguments);
  static Value* hsvvalue(const std::vector<const Value*>& arguments);
  static Value* fade(const std::vector<const Value*>& arguments);
  static Value* luma(const std::vector<const Value*>& arguments);
  static Value* luminance(const std::vector<const Value*>& arguments);
  static Value* mix(const std::vector<const Value*>& arguments);
  static Value* tint(const std::vector<const Value*>& arguments);
  static Value* shade(const std::vector<const Value*>& arguments);
  static Value* greyscale(const std::vector<const Value*>& arguments);
  static Value* contrast(const std::vector<const Value*>& arguments);

  static Value* multiply(const std::vector<const Value*>& arguments);
  static Value* screen(const std::vector<const Value*>& arguments);
  static Value* overlay(const std::vector<const Value*>& arguments);
  static Value* softlight(const std::vector<const Value*>& arguments);
  static Value* hardlight(const std::vector<const Value*>& arguments);
  static Value* difference(const std::vector<const Value*>& arguments);
  static Value* exclusion(const std::vector<const Value*>& arguments);
  static Value* average(const std::vector<const Value*>& arguments);
  static Value* negation(const std::vector<const Value*>& arguments);
};
  
#endif  // __less_value_ColorFunctions_h__
