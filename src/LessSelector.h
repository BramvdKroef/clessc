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
#include "Extension.h"

#include <list>
#include <map>
#include <iterator>

class LessSelector: public Selector {
private:
  std::list<Extension> extensions;
  std::list<string> parameters;
  std::list<TokenList> defaults;
  std::list<TokenList> conditions;

  bool _unlimitedArguments;
  bool _needsArguments;
  std::string restIdentifier;

  bool parseExtension(Selector &selector, Selector &extension);
  bool parseArguments(TokenList &selector);
  bool validateArguments(const TokenList &arguments, const std::string &delimiter);
  bool parseParameter(TokenList &selector, const std::string &delimiter);
  bool parseDefaultValue(TokenList &arguments,
                         const std::string &delimiter,
                         TokenList &value);
  bool parseConditions (TokenList &selector);
  
public:
  LessSelector(const Selector &original);
  virtual ~LessSelector();
  
  std::list<Extension>& getExtensions();
  std::list<std::string>& getParameters();
  TokenList* getDefault(const std::string &parameter);

  std::list<TokenList>& getConditions();
  bool matchArguments(const Mixin &arguments);

  bool needsArguments();
  bool unlimitedArguments();
  std::string getRestIdentifier();
  
};

#endif
