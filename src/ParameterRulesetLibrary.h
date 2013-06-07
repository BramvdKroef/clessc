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

#ifndef __ParameterRulesetLibrary_h__
#define __ParameterRulesetLibrary_h__

#include "value/ValueProcessor.h"
#include "Stylesheet.h"
#include "ParameterRuleset.h"
#include <vector>
#include <list>

class ParameterRulesetLibrary {
private:
  vector<ParameterRuleset*> parameterRulesets;
  ValueProcessor* valueProcessor;
  void processRuleset(vector<Declaration*>* declarations);
  
public:
  ParameterRulesetLibrary(ValueProcessor* valueProcessor);
  virtual ~ParameterRulesetLibrary();

  void addRule(ParameterRuleset* rule);
  
  bool parameterRulesetExists(ParameterMixin* mixin);
  bool processParameterMixin(ParameterMixin* mixin,
                             Ruleset* target,
                             Stylesheet* stylesheet);
  bool insertParameterRuleset(ParameterRuleset* pruleset,
                              ParameterMixin* mixin,
                              Ruleset* target,
                              Stylesheet* stylesheet);

};

#endif

