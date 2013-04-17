/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

#ifndef __Color_h__
#define __Color_h__

#include "Value.h"
#include "NumberValue.h"
#include "StringValue.h"
#include "FunctionLibrary.h"
#include <algorithm>
using namespace std;

#define RGB_RED 0
#define RGB_GREEN 1
#define RGB_BLUE 2
//#define ABS(x) (x < 0 ? 0 - x : x)

class Color: public Value {
private:
  unsigned int color[3];
  double alpha;
  /**
   * If set to true getTokens() will generate new tokens with the
   * updated color data.
   */
  bool valueChanged;
  
  double maxArray(double* array, int len);
  double minArray(double* array, int len);
public:
  Color(Token* token);
  Color(unsigned int red, unsigned int green, unsigned int blue);
  Color(unsigned int red, unsigned int green, unsigned int blue,
        double alpha);
  virtual ~Color();

  virtual TokenList* getTokens();
  
  virtual Value* add(Value* v);
  virtual Value* substract(Value* v);
  virtual Value* multiply(Value* v);
  virtual Value* divide(Value* v);
  virtual int compare(Value* v);

  /**
   * The HSL to RGB conversion on
   * http://en.wikipedia.org/wiki/HSL_and_HSV did not work at all so
   * the calculations are from
   * http://130.113.54.154/~monger/hsl-rgb.html, which does not list a
   * source. 
   */
  void setHSL(double hue, double saturation, double lightness);
  /**
   * Converts the internal RGB value to HSL. The source of the
   * calculations is http://en.wikipedia.org/wiki/HSL_and_HSV except
   * for the saturation value, which did not work.
   */
  double* getHSL();
  /**
   * Returns the the amount of red in the color.
   *
   * @return an int value between 0-255
   */
  unsigned int getRed();
  /**
   * Returns the the amount of green in the color.
   *
   * @return an int value between 0-255
   */
  unsigned int getGreen();
  /**
   * Returns the the amount of blue in the color.
   *
   * @return an int value between 0-255
   */
  unsigned int getBlue();

  void setAlpha(double alpha);
  double getAlpha();
  
  static void loadFunctions(FunctionLibrary* lib);
  static Value* rgb(vector<Value*> arguments);
  static Value* rgba(vector<Value*> arguments);
  static Value* lighten(vector<Value*> arguments);
  static Value* darken(vector<Value*> arguments);
  static Value* saturate(vector<Value*> arguments);
  static Value* desaturate(vector<Value*> arguments);
  static Value* fadein(vector<Value*> arguments);
  static Value* fadeout(vector<Value*> arguments);
  static Value* spin(vector<Value*> arguments);
  static Value* hsl(vector<Value*> arguments);
  static Value* hue(vector<Value*> arguments);
  static Value* saturation(vector<Value*> arguments);
  static Value* lightness(vector<Value*> arguments);

  static Value* hsla(vector<Value*> arguments);
  static Value* hsv(vector<Value*> arguments);
  static Value* hsva(vector<Value*> arguments);
  static Value* hsvhue(vector<Value*> arguments);
  static Value* hsvsaturation(vector<Value*> arguments);
  static Value* hsvvalue(vector<Value*> arguments);
  static Value* red(vector<Value*> arguments);
  static Value* green(vector<Value*> arguments);
  static Value* blue(vector<Value*> arguments);
  static Value* _alpha(vector<Value*> arguments);
  static Value* luma(vector<Value*> arguments);
  static Value* fade(vector<Value*> arguments);
  static Value* mix(vector<Value*> arguments);
  static Value* greyscale(vector<Value*> arguments);
  static Value* contrast(vector<Value*> arguments);
  static Value* screen(vector<Value*> arguments);
  static Value* overlay(vector<Value*> arguments);
  static Value* softlight(vector<Value*> arguments);
  static Value* hardlight(vector<Value*> arguments);
  static Value* difference(vector<Value*> arguments);
  static Value* exclusion(vector<Value*> arguments);
  static Value* avarage(vector<Value*> arguments);
  static Value* negation(vector<Value*> arguments);
};

#endif
