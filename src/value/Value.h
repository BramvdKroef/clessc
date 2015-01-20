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

#ifndef __Value_h__
#define __Value_h__

#include "../Token.h"
#include "../TokenList.h"
#include "ValueException.h"

class BooleanValue;

/**
 * 
 */
class Value {
protected:
  TokenList tokens;
  
public:
  enum Type {NUMBER, PERCENTAGE, DIMENSION, COLOR, STRING, UNIT,
             BOOLEAN, URL} type;
  Value();
  Value(Token &token);
  virtual ~Value();
  
  virtual const TokenList* getTokens() const;
  
  virtual Value* add(const Value &v) const =0;
  virtual Value* substract(const Value &v) const =0;
  virtual Value* multiply(const Value &v) const =0;
  virtual Value* divide(const Value &v) const =0;

  virtual BooleanValue* lessThan(const Value &v) const;
  virtual BooleanValue* equals(const Value &v) const;

  BooleanValue* greaterThan(const Value &v) const;
  BooleanValue* lessThanEquals(const Value &v) const;
  BooleanValue* greaterThanEquals(const Value &v) const;

  static const char* typeToString(const Type &t);
  /**
   * return a type for a type code.
   * N - Number
   * P - Percentage
   * D - Dimension
   * C - Color
   * S - String
   * U - Unit
   * B - Boolean
   * R - URL
   */
  static Type codeToType(const char code);

};

  
#include "BooleanValue.h"


#endif
