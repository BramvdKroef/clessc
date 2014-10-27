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

#ifndef __LessSelector_h__
#define __LessSelector_h__


#include "Selector.h"
#include "Mixin.h"

#include <list>

class LessSelector: public Selector {
private:
  Selector* original;

  map<string, TokenList*> extensions;
  list<string> parameters;
  list<TokenList*> defaults;
  list<TokenList*> conditions;

  bool _unlimitedArguments;
  bool _needsArguments;
  string restIdentifier;

  TokenList* parseExtension(TokenList* selector);
  bool parseArguments(TokenList* selector);
  bool validateArguments(TokenList* arguments, string delimiter);
  bool parseParameter(TokenList* selector, string delimiter);
  TokenList* parseDefaultValue(TokenList* arguments,
                               string delimiter);
  bool parseConditions (TokenList* selector);
  
public:
  LessSelector(Selector* original);
  virtual ~LessSelector();

  
  map<string, TokenList*>* getExtensions();
  list<string>* getParameters();
  TokenList* getDefault(string parameter);

  list<TokenList*>* getConditions();
  bool matchArguments(Mixin* arguments);

  bool needsArguments();
  bool unlimitedArguments();
  string getRestIdentifier();
  
};

#endif
