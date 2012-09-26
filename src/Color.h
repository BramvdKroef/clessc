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
#include <algorithm>
using namespace std;

#define RGB_RED 0
#define RGB_GREEN 1
#define RGB_BLUE 2
#define abs(x) (x < 0 ? 0 - x : x)

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
  
  void add(Value* v);
  void substract(Value* v);
  void multiply(Value* v);
  void divide(Value* v);

  /**
   * The HSL to RGB conversion on
   * http://en.wikipedia.org/wiki/HSL_and_HSV did not work at all so
   * the calculations are from
   * http://130.113.54.154/~monger/hsl-rgb.html, which does not list a
   * source. 
   */
  void setHSL(double hue, double saturation, double lightness);
  void lighten(double percent);
  void darken(double percent);
  void saturate(double percent);
  void desaturate(double percent);
  void fadein(double percent);
  void fadeout(double percent);
  void spin(double degrees);

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
  
  /**
   * Converts the internal RGB value to HSL. The source of the
   * calculations is http://en.wikipedia.org/wiki/HSL_and_HSV except
   * for the saturation value, which did not work.
   */
  double* getHSL();

  /**
   * Find the hue value for the color.
   * @see getHSL();
   */
  int getHue();

  /**
   * Find the saturation value for the color.
   * @see getHSL();
   */
  int getSaturation();

  /**
   * Find the lightness value for the color.
   * @see getHSL();
   */
  int getLightness();
};

#endif
