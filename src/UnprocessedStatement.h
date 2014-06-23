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

#ifndef __UnprocessedStatement_h__
#define __UnprocessedStatement_h__

#include "Stylesheet.h"
#include "TokenList.h"
#include "value/ValueProcessor.h"


class UnprocessedStatement: public RulesetStatement {
private:
  Selector tokens;
    
public:
  size_t property_i;
  static const int UNPROCESSED = 2;
  static const int PROCESSED = 3;
  int type;

  unsigned int line, column;
    
  UnprocessedStatement();

  Selector* getTokens();

  void getProperty(TokenList* tokens);
  void getValue(TokenList* tokens);

  bool processVariable(ValueProcessor* valueProcessor);
  bool processDeclaration (Declaration* declaration);
  
  virtual int getType();
  virtual RulesetStatement* clone();
};

#endif
  
