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

#include "../TokenList.h"
#include "../Token.h"
#include "../ParseException.h"
#include "Value.h"
#include "Color.h"
#include "NumberValue.h"
#include "StringValue.h"
#include "UnitValue.h"
#include "UrlValue.h"
#include "ValueException.h"
#include "ValueScope.h"
#include "FunctionLibrary.h"
#include <map>
#include <vector>
#include <cstring>
/**
 * 
 */
class ValueProcessor {
private:
  FunctionLibrary functionLibrary;

  Value* processStatement(TokenList& value, const ValueScope &scope);

  Value* processOperator(TokenList& value, const Value &operand1,
                         const ValueScope &scope, Token* lastop = NULL);

  Value* processConstant(TokenList &value, const ValueScope& scope);

  const TokenList* processDeepVariable (TokenList &value, const ValueScope& scope);
  
  Value* processFunction(const std::string &function,
                         TokenList &value,
                         const ValueScope &scope);
  
  vector<Value*> processArguments (TokenList &value,
                                   const ValueScope &scope);
  Value* processEscape (TokenList &value,
                        const ValueScope &scope);
  UnitValue* processUnit(Token &t);
  
  bool needsSpace(const Token &t, bool before);

  Value* processNegative(TokenList &value,
                               const ValueScope &scope);
  
public:
  ValueProcessor();
  virtual ~ValueProcessor();


  /**
   * Determine if a value contains anything that can be processed.
   *
   * @return  true if value contains an @-keyword, an operator (+-* or
   *          /), a LESS function, an escaped value or a url.
   */
  bool needsProcessing(const TokenList &value);

  void processValue(TokenList &value, const ValueScope &scope);

  bool validateCondition(TokenList &value, const ValueScope &scope);
  bool validateValue(TokenList &value, const ValueScope &scope);
  
  bool functionExists(const string &function);

  void interpolate(string &str, const ValueScope &scope);
  void interpolate(TokenList &tokens, const ValueScope &scope);
};

#endif
