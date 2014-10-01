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
#include "ParameterRuleset.h"
#include "ParameterMixin.h"
#include "UnprocessedStatement.h"

class LessStylesheet: public Stylesheet {
private:
  ValueProcessor valueProcessor;
  vector<LessRuleset*> lessrulesets;
  vector<ParameterRuleset*> parameterRulesets;
  
public:
  LessStylesheet();
  virtual ~ LessStylesheet();
  
  void process();

  void addStatement(LessRuleset* ruleset);

  void putVariable(string key, TokenList* value);
  void addParameterRuleset(ParameterRuleset* rule);  
  bool hasParameterRuleset(ParameterMixin* mixin);

protected:
  void processRuleset(LessRuleset* ruleset);
  void processVariables(map<string, TokenList*>* variables) ;
  void processStatements(vector<RulesetStatement*>*
                         statements, Ruleset* target);
  void processStatement(UnprocessedStatement* statement,
                        Ruleset* target);

  void insertNestedRules(LessRuleset* parent, Selector* prefix);
    
  bool processParameterMixin(ParameterMixin* mixin,
                             Ruleset* target);
  bool insertParameterRuleset(ParameterRuleset* pruleset,
                              list<TokenList*>* arguments,
                              Ruleset* target);
};

#endif
