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
#include "FunctionLibrary.h"
#include "UnitValue.h"
#include <vector>

class NumberValue: public Value {

public:
  NumberValue(Token* token);
  virtual ~NumberValue();
  
  Value* add(Value* v);
  Value* substract(Value* v);
  Value* multiply(Value* v);
  Value* divide(Value* v);

  void setType(Value* v);
  
  string getUnit();
  void setUnit(string unit);
  double getValue();
  void setValue(double d);

  static void loadFunctions(FunctionLibrary* lib);
  static Value* unit(vector<Value*> args);
  void ceil();
  void floor();
  void percentage();
  void round();
  void sqrt();
  void abs();
  void sin();
  void asin();
  void cos();
  void acos();
  void tan();
  void atan();
  void pi();
  void pow(Value* v);
  void mod(Value* v);
  void convert(Value* unit);
};

#endif
