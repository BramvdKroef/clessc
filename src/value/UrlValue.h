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

using namespace std;

class UrlValue: public Value {
private:

  string path;
  
  bool loaded;
  int width;
  int height;
  
  bool loadImg();
  bool loadPng();
  bool loadJpeg();

public:
  UrlValue(Token* token, string path);

  virtual ~UrlValue();
  
  virtual Value* add(Value* v);
  virtual Value* substract(Value* v);
  virtual Value* multiply(Value* v);
  virtual Value* divide(Value* v);
  virtual int compare(Value* v);

  unsigned int getImageWidth();
  unsigned int getImageHeight();

  static void loadFunctions(FunctionLibrary* lib);

  static Value* imgheight(vector<Value*> arguments);
  static Value* imgwidth(vector<Value*> arguments);
};
  
#endif
