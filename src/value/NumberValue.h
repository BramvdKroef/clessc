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

#ifndef __NumberValue_h__
#define __NumberValue_h__

#include "Value.h"
#include "StringValue.h"
#include "UnitValue.h"
#include <vector>
#include <cmath>
class FunctionLibrary;

class NumberValue: public Value {
  static bool isNumber(Value* val);

  void verifyUnits(NumberValue* v);
  bool convert(string unit);
  
public:
  NumberValue(Token* token);
  virtual ~NumberValue();
  
  virtual Value* add(Value* v);
  virtual Value* substract(Value* v);
  virtual Value* multiply(Value* v);
  virtual Value* divide(Value* v);
  virtual int compare(Value* v);

  void setType(Value* v);
  
  string getUnit();
  void setUnit(string unit);
  double getValue();
  void setValue(double d);

  static void loadFunctions(FunctionLibrary* lib);
  static Value* unit(vector<Value*> args);
  static Value* ceil(vector<Value*> args);
  static Value* floor(vector<Value*> args);
  static Value* percentage(vector<Value*> args);
  static Value* round(vector<Value*> args);
  static Value* sqrt(vector<Value*> args);
  static Value* abs(vector<Value*> args);
  static Value* sin(vector<Value*> args);
  static Value* asin(vector<Value*> args);
  static Value* cos(vector<Value*> args);
  static Value* acos(vector<Value*> args);
  static Value* tan(vector<Value*> args);
  static Value* atan(vector<Value*> args);
  static Value* pi(vector<Value*> args);
  static Value* pow(vector<Value*> args);
  static Value* mod(vector<Value*> args);
  static Value* convert(vector<Value*> args);
};

#endif
