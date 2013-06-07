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

#include "ParameterRulesetLibrary.h"

ParameterRulesetLibrary::ParameterRulesetLibrary(ValueProcessor* valueProcessor) {
  this->valueProcessor = valueProcessor;
}
ParameterRulesetLibrary::~ParameterRulesetLibrary() {
  while (!parameterRulesets.empty()) {
    delete parameterRulesets.back();
    parameterRulesets.pop_back();
  }
}

void ParameterRulesetLibrary::addRule(ParameterRuleset* rule) {
  parameterRulesets.push_back(rule);
}

bool ParameterRulesetLibrary::parameterRulesetExists(ParameterMixin* mixin) {
  vector<ParameterRuleset*>::iterator pri;
  
  for (pri = parameterRulesets.begin(); pri < parameterRulesets.end();
       pri++) {
    
    if ((*pri)->getSelector()->equals(mixin->name) &&
        (*pri)->matchArguments(mixin->arguments)) {
      return true;
    }
  }
  return false;
}

bool ParameterRulesetLibrary::processParameterMixin(ParameterMixin* mixin,
                                       Ruleset* target,
                                       Stylesheet* stylesheet) {
  vector<ParameterRuleset*>::iterator pri;
  ParameterRuleset* pruleset;
  bool ret = false;
  
  for (pri = parameterRulesets.begin(); pri < parameterRulesets.end();
       pri++) {
    pruleset = *pri;
    
    if (pruleset->getSelector()->equals(mixin->name) &&
        pruleset->matchArguments(mixin->arguments)) {

      ret = insertParameterRuleset(pruleset, mixin,
                                   target, stylesheet) || ret;
    }
  }
  
  return ret;
}

bool ParameterRulesetLibrary::insertParameterRuleset(ParameterRuleset* pruleset,
                                        ParameterMixin* mixin,
                                        Ruleset* target,
                                        Stylesheet* stylesheet) {
  vector<Declaration*>* declarations;
  list<Ruleset*>* nestedRules;
  list<Ruleset*>::iterator r_it;
  Ruleset* nestedRule;
  list<ParameterMixin*>* mixins;
  list<ParameterMixin*>::iterator m_it;
  
  bool ret = false;
  
  // new scope
  valueProcessor->pushScope();

  if (pruleset->putArguments(valueProcessor, mixin->arguments) &&
      pruleset->matchConditions(valueProcessor)) {

    // declarations
    if (target != NULL) {
      declarations = pruleset->cloneDeclarations();
      processRuleset(declarations);
      target->addDeclarations(declarations);
      delete declarations;
    }
    
    // mixin calls
    mixins = pruleset->getMixins();
    for (m_it = mixins->begin(); m_it != mixins->end(); m_it++) {
      if (target != NULL && !target->getSelector()->empty()) 
        (*m_it)->prefix->addPrefix(target->getSelector());
      
      processParameterMixin((*m_it), target, stylesheet);
    }

    // nested rules
    nestedRules = pruleset->getNestedRules();
    for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
      nestedRule = (*r_it)->clone();
      processRuleset(nestedRule->getDeclarations());

      if (!mixin->prefix->empty())
        nestedRule->getSelector()->addPrefix(mixin->prefix);

      stylesheet->addRuleset(nestedRule);
    }

    ret = true;
  }
      
  valueProcessor->popScope();

  return ret;
}

void ParameterRulesetLibrary::processRuleset(vector<Declaration*>* declarations) {
  vector<Declaration*>::iterator it;
  for(it = declarations->begin(); it != declarations->end(); ++it) {
    valueProcessor->processValue((*it)->getValue());
  }
}
