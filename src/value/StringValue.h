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

#ifndef __StringValue_h__
#define __StringValue_h__

#include "Value.h"
#include "Color.h"
#include "FunctionLibrary.h"
#include <vector>
#include <sstream>
#include <iomanip>

class StringValue: public Value {
private:
  std::string strvalue;
  bool quotes;

  void updateTokens();

public:
  StringValue(Token &token);
  StringValue(Token &token, bool quotes);
  StringValue(std::string &str, bool quotes);
  StringValue(const StringValue &s);
  StringValue(const Value &val, bool quotes);
  
  virtual ~StringValue();
  
  std::string getString() const;
  void setString(const std::string &stringValue);
  
  void setQuotes(bool quotes);
  bool getQuotes() const;

  void append(const Value &v);

  virtual Value* add(const Value &v) const;
  virtual Value* substract(const Value &v) const;
  virtual Value* multiply(const Value &v) const;
  virtual Value* divide(const Value &v) const;

  virtual BooleanValue* equals(const Value &v) const;
  virtual BooleanValue* lessThan(const Value &v) const;

  static string escape(string rawstr, string extraUnreserved = "");

  static void loadFunctions(FunctionLibrary &lib);
  static Value* escape(vector<Value*> arguments);
  static Value* e(vector<Value*> arguments);
  static Value* format(vector<Value*> arguments);
  static Value* color(vector<Value*> arguments);
  static Value* data_uri(vector<Value*> arguments);
};
#endif
