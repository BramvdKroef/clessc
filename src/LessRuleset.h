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

#ifndef __LessRuleset_h__
#define __LessRuleset_h__

#include "Stylesheet.h"
#include "Selector.h"
#include "ParseException.h"
#include "value/ValueProcessor.h"
#include "UnprocessedStatement.h"
#include "LessSelector.h"
#include "ParameterMixin.h"
#include <map>
#include <list>

class LessStylesheet;

class LessRuleset: public Ruleset {
  
protected:
  map<string, TokenList*> variables;  
  list<LessRuleset*> nestedRules;
  list<UnprocessedStatement*> unprocessedStatements;
  LessRuleset* parent;
  LessStylesheet* lessStylesheet;
  LessSelector* selector;

  void processVariables();
  void insertNestedRules(Stylesheet* s, Selector* prefix);
  
public:
  LessRuleset();
  LessRuleset(Selector* selector);
  virtual ~LessRuleset();

  virtual void setSelector(Selector* selector);

  virtual void addStatement (UnprocessedStatement* statement);
  list<UnprocessedStatement*>* getUnprocessedStatements();
  
  void addNestedRule(LessRuleset* nestedRule);
  list<LessRuleset*>* getNestedRules();

  void putVariable(string key, TokenList* value);
  map<string, TokenList*>* getVariables();

  void setParent(LessRuleset* r);
  LessRuleset* getParent();

  virtual void setStylesheet(LessStylesheet* stylesheet);
  LessStylesheet* getLessStylesheet();

  void getExtensions(map<string, TokenList*>* extensions);
  
  virtual bool insert(list<TokenList*>* arguments, Ruleset* target);
  virtual bool insert(list<TokenList*>* arguments, Stylesheet* s);

  virtual void process(Stylesheet* s);
  virtual void process(Stylesheet* s, Selector* prefix);
  
  void getLessRulesets(list<LessRuleset*>* rulesetList,
                       ParameterMixin* mixin,
                       size_t selector_offset);
  bool matchConditions();
  bool putArguments(list<TokenList*>* arguments);
};

#endif
