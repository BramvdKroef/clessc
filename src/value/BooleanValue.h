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

#ifndef __BooleanValue_h__
#define __BooleanValue_h__

#include "Value.h"
#include "ValueException.h"
#include "StringValue.h"

/**
 * True or false.
 */
class BooleanValue: public Value {
private:
  bool value;
public:
  BooleanValue(bool value);
  virtual ~BooleanValue();

  bool getValue();
  void setValue(bool value);

  virtual Value* add(Value* v);
  virtual Value* substract(Value* v);
  virtual Value* multiply(Value* v);
  virtual Value* divide(Value* v);
  virtual int compare(Value* v);
};
#endif
  
