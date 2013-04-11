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

#ifndef __ValueProcessor_h__
#define __ValueProcessor_h__

#include "TokenList.h"
#include "Token.h"
#include "Value.h"
#include "Color.h"
#include "ValueException.h"
#include <map>
#include <vector>

/**
 * 
 */
class ValueProcessor {
private:
  list<map<string, TokenList*>*> scopes;
  
  Value* processStatement(TokenList* value);
  Value* processOperator(TokenList* value, Value* v1,
                         Token* lastop = NULL);
  Value* processConstant(TokenList* value);
  TokenList* processDeepVariable (TokenList* value);
  Value* processFunction(Token* function,
                         TokenList* value);
  vector<Value*> processArguments (TokenList* value);
  bool checkTypes(vector<Value*> arguments, const char* types, unsigned int len);
  void processString(Token* str);
  Token* processEscape (TokenList* value);
  bool needsSpace(Token* t, bool suffix);
  
public:
  ValueProcessor();
  virtual ~ValueProcessor();

  void putVariable(string key, TokenList* value);
  TokenList* getVariable(string key);
  TokenList* processValue(TokenList* value);

  void pushScope();
  void popScope();
};

#endif
