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
#include "ProcessingContext.h"
#include "LessAtRule.h"

#include <list>
#include <map>
#include <string>

class LessMediaQuery;

class LessStylesheet: public Stylesheet {
private:
  std::list<LessRuleset*> lessrulesets;
  std::map<std::string, TokenList> variables;
  ProcessingContext* context;
  
public:
  LessStylesheet();
  virtual ~LessStylesheet();

  LessRuleset* createLessRuleset();
  Mixin* createMixin();
  LessAtRule* createLessAtRule(std::string &keyword);
  LessMediaQuery* createLessMediaQuery();

  void deleteLessRuleset(LessRuleset &ruleset);
  void deleteMixin(Mixin &mixin);
  
  virtual void getLessRulesets(std::list<LessRuleset*> &rulesetList,
                               const Mixin &mixin);

  void setContext(ProcessingContext* context);
  virtual ProcessingContext* getContext();
  
  void putVariable(const std::string &key, const TokenList &value);

  virtual void process(Stylesheet &s, ProcessingContext &context);

  
};

#endif
