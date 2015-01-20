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

#ifndef __UnitValue_h__
#define __UnitValue_h__

#include "Value.h"
#include "ValueException.h"
#include "StringValue.h"

/**
 * A dimension unit: em,ex,px,ch,in,cm,mm,pt,pc
 */
class UnitValue: public Value {
public:
  enum UnitGroup {NO_GROUP, LENGTH, TIME, ANGLE};
  
  UnitValue(Token &token);
  virtual ~UnitValue();

  const char* getUnit() const;
  
  virtual Value* add(const Value &v) const;
  virtual Value* substract(const Value &v) const;
  virtual Value* multiply(const Value &v) const;
  virtual Value* divide(const Value &v) const;

  virtual BooleanValue* lessThan(const Value &v) const;
  virtual BooleanValue* equals(const Value &v) const;

  static UnitGroup getUnitGroup(const std::string &unit);
  static double lengthToPx(const double length, const std::string &unit);
  static double pxToLength(double px, const std::string &unit);
  
  static double timeToMs(double time, const std::string &unit);
  static double msToTime(double ms, const std::string &unit);
  
  static double angleToRad(double angle, const std::string &unit);
  static double radToAngle(double rad, const std::string &unit);
};

#endif
