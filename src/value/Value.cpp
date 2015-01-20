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

#include "Value.h"
#include "BooleanValue.h"

Value::Value() {
}

Value::~Value() {
}

const TokenList* Value::getTokens() const{
  return &tokens;
}

BooleanValue* Value::lessThan(const Value &v) const {
  return new BooleanValue(this->type < v.type ||
                          (this->type == v.type &&
                           getTokens() < v.getTokens()));
}


BooleanValue* Value::equals(const Value &v) const {
  return new BooleanValue(this->type == v.type &&
                          getTokens() == v.getTokens());
}

BooleanValue* Value::greaterThan(const Value &v) const {
  BooleanValue* ret = this->equals(v);

  if (ret->getValue()) {
    ret->setValue(false);
    return ret;
  }
  delete ret;
  
  ret = this->lessThan(v);
  ret->setValue(!ret->getValue());

  return ret;
}
BooleanValue* Value::lessThanEquals(const Value &v) const {
  BooleanValue* ret = this->equals(v);

  if (ret->getValue())
    return ret;

  delete ret;

  ret = this->lessThan(v);
  return ret;
}
BooleanValue* Value::greaterThanEquals(const Value &v) const {
  BooleanValue* ret = this->equals(v);

  if (ret->getValue())
    return ret;

  delete ret;

  ret = this->lessThan(v);
  ret->setValue(!ret->getValue());
  return ret;
}

const char* Value::typeToString(const Type &t) {
  switch (t) {
  case NUMBER:
    return "Number";
  case PERCENTAGE:
    return "Percentage";
  case DIMENSION:
    return "Dimension";
  case COLOR:
    return "Color";
  case STRING:
    return "String";
  case UNIT:
    return "Unit";
  case BOOLEAN:
    return "Boolean";
  case URL:
    return "URL";
  }
  return "Undefined";
}
Value::Type Value::codeToType(const char code) {
  switch (code) {
  case 'N':
    return NUMBER;
  case 'P':
    return PERCENTAGE;
  case 'D':
    return DIMENSION;
  case 'C':
    return COLOR;
  case 'U':
    return UNIT;
  case 'S':
    return STRING;
  case 'B':
    return BOOLEAN;
  case 'R':
    return URL;
  default:
    return NUMBER;
  }
}

