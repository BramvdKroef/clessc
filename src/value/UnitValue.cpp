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

UnitValue::UnitValue(Token &token) {
  tokens.push_back(token);
  type = UNIT;
}

UnitValue::~UnitValue() {
}

const char* UnitValue::getUnit() const {
  return tokens.front().c_str();
}

Value* UnitValue::add(const Value &v) const {
  Token t;
  const StringValue* s;
  StringValue* ret;

  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    t = this->tokens.front();
    t.type = Token::STRING;
    ret = new StringValue(t, s->getQuotes());
    ret->add(v);
    return ret;
  }
  throw new ValueException("Can't do math on unit types.");
}
Value* UnitValue::substract(const Value &v) const {
  (void)v;
  throw new ValueException("Can't do math on unit types.");
}
Value* UnitValue::multiply(const Value &v) const {
  (void)v;
  throw new ValueException("Can't do math on unit types.");
}
Value* UnitValue::divide(const Value &v) const {
  (void)v;
  throw new ValueException("Can't do math on unit types.");
}

BooleanValue* UnitValue::lessThan(const Value &v) const {
  const UnitValue* u;
  
  if (v.type == UNIT) {
    u = static_cast<const UnitValue*>(&v);
    return new BooleanValue(getUnit() < u->getUnit());
  } else {
    throw new ValueException("You can only compare a unit with a *unit*.");
  }
}
BooleanValue* UnitValue::equals(const Value &v) const {
  const UnitValue* u;
  
  if (v.type == UNIT) {
    u = static_cast<const UnitValue*>(&v);
    return new BooleanValue(getUnit() == u->getUnit());
  } else {
    throw new ValueException("You can only compare a unit with a *unit*.");
  }
}


UnitValue::UnitGroup UnitValue::getUnitGroup(const string &unit) {
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

double UnitValue::lengthToPx(const double length, const std::string &unit) {
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
double UnitValue::pxToLength(const double px, const std::string &unit) {
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
double UnitValue::timeToMs(const double time, const std::string &unit) {
  if (unit.compare("s") == 0)
    return time * 1000;
  if (unit.compare("ms") == 0)
    return time;
  return -1;
}
double UnitValue::msToTime(const double ms, const std::string &unit) {
  if (unit.compare("s") == 0)
    return ms / 1000;
  if (unit.compare("ms") == 0)
    return ms;
  return -1;
}
double UnitValue::angleToRad(const double angle, const std::string &unit) {
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
double UnitValue::radToAngle(const double rad, const std::string &unit) {
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
