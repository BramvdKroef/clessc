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

class StringValue: public Value {
private:
  string stringvalue;
  bool quotes;

public:
  StringValue(Token* token);
  StringValue(Token* token, bool quotes);
  virtual ~StringValue();
  
  virtual TokenList* getTokens();

  void setQuotes(bool quotes);
  bool getQuotes();
  
  Value* add(Value* v);
  Value* substract(Value* v);
  Value* multiply(Value* v);
  Value* divide(Value* v);

  static void loadFunctions(FunctionLibrary* lib);
  static Value* escape(vector<Value*> arguments);
  static Value* e(vector<Value*> arguments);
  static Value* format(vector<Value*> arguments);
  static Value* color(vector<Value*> arguments);
  static Value* data_uri(vector<Value*> arguments);
};
#endif
