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

#ifndef __ParameterRuleset_h__
#define __ParameterRuleset_h__

#include "Stylesheet.h"
#include "Selector.h"
#include "ParseException.h"
#include "value/ValueProcessor.h"
#include <map>


class ParameterRuleset: public Ruleset {
private:
  string rest;
  bool unlimitedArguments;
  
protected:
  bool processParameter(Selector* selector);
    
public:
  list<string> parameters;
  list<TokenList*> defaults;
  list<Ruleset*> nestedRules;
  
  static bool isValid(Selector* selector);
  
  ParameterRuleset(Selector* selector);
  ~ParameterRuleset();
  
  void addParameter(string keyword, TokenList* value);

  TokenList* getDefault(string keyword);

  list<string> getKeywords();

  bool matchArguments(list<TokenList*>* arguments);
  bool matchConditions(ValueProcessor* valueProcessor);
  
  bool putArguments(ValueProcessor* valueProcessor,
                    list<TokenList*>* arguments);

  void addNestedRule(Ruleset* nestedRule);
  list<Ruleset*>* getNestedRules();

};

#endif
