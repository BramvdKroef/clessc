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

#include "UnitValue.h"

UnitValue::UnitValue(Token* token) {
  tokens.push(token);
  type = UNIT;
}

UnitValue::~UnitValue() {
}

const char* UnitValue::getUnit() {
  return tokens.front()->str.c_str();
}

Value* UnitValue::add(Value* v) {
  Token* t;
  StringValue* s;

  if (v->type == STRING) {
    t = this->tokens.front()->clone();
    t->type = Token::STRING;
    s = new StringValue(t, ((StringValue*)v)->getQuotes());
    s->add(v);
    return s;
  }
  throw new ValueException("Can't do math on unit types.");
}
Value* UnitValue::substract(Value* v) {
  (void)v;
  throw new ValueException("Can't do math on unit types.");
}
Value* UnitValue::multiply(Value* v) {
  (void)v;
  throw new ValueException("Can't do math on unit types.");
}
Value* UnitValue::divide(Value* v) {
  (void)v;
  throw new ValueException("Can't do math on unit types.");
}
int UnitValue::compare(Value* v) {
  string unit1, unit2;
  if (v->type == UNIT) {
    unit1.append(getUnit());
    unit2.append(((UnitValue*)v)->getUnit());
    return unit1.compare(unit2);
  } else {
    throw new ValueException("You can only compare a unit with a *unit*.");
  }
}


UnitValue::UnitGroup UnitValue::getUnitGroup(const string unit) {
  if (unit.compare("m") == 0 ||
      unit.compare("cm") == 0 ||
      unit.compare("mm") == 0 ||
      unit.compare("in") == 0 ||
      unit.compare("pt") == 0 ||
      unit.compare("pc") == 0 ||
      unit.compare("px") == 0) {
    return LENGTH;
  }
  if (unit.compare("s") == 0 ||
      unit.compare("ms") == 0) {
    return TIME;
  }
  if (unit.compare("rad") == 0 ||
      unit.compare("deg") == 0 ||
      unit.compare("grad") == 0 ||
      unit.compare("turn") == 0) {
    return ANGLE;
  }
  return NO_GROUP;
}

double UnitValue::lengthToPx(const double length, const string unit) {
  if (unit.compare("m") == 0)
    return length * (96 / .0254);
  
  if (unit.compare("cm") == 0)
    return length * (96 / 2.54);
  
  if (unit.compare("mm") == 0)
    return length * (96 / 25.4);
  
  if (unit.compare("in") == 0)
    return length * 96;
  
  if (unit.compare("pt") == 0)
    return length * (4 / 3);
  
  if (unit.compare("pc") == 0)
    return length * 16;
  
  if (unit.compare("px") == 0)
    return length;

  return -1;
}
double UnitValue::pxToLength(const double px, const string unit) {
  if (unit.compare("m") == 0)
    return px / (96 / .254);
  
  if (unit.compare("cm") == 0)
    return px / (96 / 2.54);
  
  if (unit.compare("mm") == 0)
    return px / (96 / 25.4);
  
  if (unit.compare("in") == 0)
    return px / 96;
  
  if (unit.compare("pt") == 0)
    return px / (4 / 3);
  
  if (unit.compare("pc") == 0)
    return px / 16;
  
  if (unit.compare("px") == 0)
    return px;

  return -1;
}
double UnitValue::timeToMs(const double time, const string unit) {
  if (unit.compare("s") == 0)
    return time * 1000;
  if (unit.compare("ms") == 0)
    return time;
  return -1;
}
double UnitValue::msToTime(const double ms, const string unit) {
  if (unit.compare("s") == 0)
    return ms / 1000;
  if (unit.compare("ms") == 0)
    return ms;
  return -1;
}
double UnitValue::angleToRad(const double angle, const string unit) {
  const double pi = 3.141592653589793;
  
  if (unit.compare("rad") == 0) 
    return angle;
  
  if (unit.compare("deg") == 0)
    return angle / 180 * pi;

  if (unit.compare("grad") == 0)
    return angle / 200 * pi;

  if (unit.compare("turn") == 0)
    return angle * 2 * pi;
  
  return -1;
}
double UnitValue::radToAngle(double rad, string unit) {
  const double pi = 3.141592653589793;
  
  if (unit.compare("rad") == 0) 
    return rad;
  
  if (unit.compare("deg") == 0)
    return rad / pi * 180;

  if (unit.compare("grad") == 0)
    return rad / pi * 200;

  if (unit.compare("turn") == 0)
    return rad / (2 * pi);
  
  return -1;
}
