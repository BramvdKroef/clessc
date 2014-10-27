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

#ifndef __LessStylesheet_h__
#define __LessStylesheet_h__

#include "Stylesheet.h"
#include "TokenList.h"
#include "LessRuleset.h"
#include "Mixin.h"
#include "UnprocessedStatement.h"

class LessMediaQuery;

class LessStylesheet: public Stylesheet {
private:
  ValueProcessor valueProcessor;
  vector<LessRuleset*> lessrulesets;
  map<string, TokenList*> variables;
  
public:
  LessStylesheet();
  virtual ~LessStylesheet();

  virtual void addStatement(AtRule* atrule);
  virtual void addStatement(LessRuleset* ruleset);
  virtual void addStatement(Mixin* mixin);
  virtual void addStatement(LessMediaQuery* query);

  virtual void getLessRulesets(list<LessRuleset*>* rulesetList,
                               Mixin* mixin);

  void getExtensions(map<string, TokenList*>* extensions);
  
  virtual ValueProcessor* getValueProcessor();
  void putVariable(string key, TokenList* value);

  virtual void process(Stylesheet* s);

  void processVariables();
};

#endif
