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
#include <iostream>

Value::Value() {
}

Value::~Value() {
}

const TokenList* Value::getTokens() const{
  return &tokens;
}
Value* Value::equals(const Value &v) {
  return new BooleanValue(this->compare(v) == 0);
}
Value* Value::lessThan(const Value &v) {
  return new BooleanValue(this->compare(v) < 0);
}
Value* Value::greaterThan(const Value &v) {
  return new BooleanValue(this->compare(v) > 0);
}
Value* Value::lessThanEquals(const Value &v) {
  return new BooleanValue(this->compare(v) <= 0);
}
Value* Value::greaterThanEquals(const Value &v) {
  return new BooleanValue(this->compare(v) >= 0);
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

