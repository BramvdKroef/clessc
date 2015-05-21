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

#ifndef __UrlValue_h__
#define __UrlValue_h__

#include "Value.h"
#include "Color.h"
#include <string>

typedef struct UrlValue_Img {
  unsigned int width;
  unsigned int height;
  Color background;
} UrlValue_Img;

class UrlValue: public Value {
private:

  std::string path;

  bool loadImg(UrlValue_Img &img) const;
  bool loadPng(UrlValue_Img &img) const;
  bool loadJpeg(UrlValue_Img &img) const;

public:
  UrlValue(Token &token, std::string &path);

  virtual ~UrlValue();

  std::string getPath() const;

  std::string getRelativePath() const;
  
  virtual Value* add(const Value &v) const;
  virtual Value* substract(const Value &v) const;
  virtual Value* multiply(const Value &v) const;
  virtual Value* divide(const Value &v) const;

  virtual BooleanValue* lessThan(const Value &v) const;
  virtual BooleanValue* equals(const Value &v) const;

  unsigned int getImageWidth() const;
  unsigned int getImageHeight() const;
  Color getImageBackground() const;

  static void loadFunctions(FunctionLibrary &lib);

  static Value* imgheight(const vector<const Value*> &arguments);
  static Value* imgwidth(const vector<const Value*> &arguments);
  static Value* imgbackground(const vector<const Value*> &arguments);
};
  
#endif
