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

#include "LessRuleset.h"
#include "LessStylesheet.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

LessRuleset::LessRuleset() : Ruleset() {
  parent = NULL;
  lessStylesheet = NULL;
  selector = NULL;
}
LessRuleset::LessRuleset(Selector* selector) : Ruleset() {
  parent = NULL;
  lessStylesheet = NULL;
  setSelector(selector);
}
LessRuleset::~LessRuleset() {
  while (!nestedRules.empty()) {
    delete nestedRules.back();
    nestedRules.pop_back();
  }
}

void LessRuleset::setSelector(Selector* selector) {
  this->selector = new LessSelector(selector);
  Ruleset::setSelector(this->selector);
}
LessSelector* LessRuleset::getLessSelector() {
  return selector;
}

void LessRuleset::addStatement(UnprocessedStatement* statement) {
  Ruleset::addStatement(statement);
  statement->setRuleset(this);
  unprocessedStatements.push_back(statement);
}
list<UnprocessedStatement*>* LessRuleset::getUnprocessedStatements() {
  return &unprocessedStatements;
}

void LessRuleset::addNestedRule(LessRuleset* nestedRule) {
#ifdef WITH_LIBGLOG
  VLOG(2) << "Adding nested rule: " <<
    nestedRule->getSelector()->toString();
#endif
  
  nestedRules.push_back(nestedRule);
  nestedRule->setParent(this);
  nestedRule->setStylesheet(getLessStylesheet());
}

list<LessRuleset*>* LessRuleset::getNestedRules() {
  return &nestedRules;
}

void LessRuleset::putVariable(string key, TokenList* value) {
  variables.insert(pair<string, TokenList*>(key, value));  
}

map<string, TokenList*>* LessRuleset::getVariables() {
  return &variables;
}
 
void LessRuleset::setParent(LessRuleset* r) {
  parent = r;
}
LessRuleset* LessRuleset::getParent() {
  return parent;
}

void LessRuleset::setStylesheet(LessStylesheet* s) {
#ifdef WITH_LIBGLOG
  VLOG(3) << "set LessStylesheet";
#endif
  lessStylesheet = s;
  Ruleset::setStylesheet(s);
}

LessStylesheet* LessRuleset::getLessStylesheet() {
  return lessStylesheet;
}

void LessRuleset::getExtensions(map<string, TokenList*>* extensions,
                                Selector* prefix) {
  
  map<string, TokenList*>::iterator e_it;
  
  list<UnprocessedStatement*>::iterator s_it;
  TokenList* extension;
  Selector* selector;

  list<LessRuleset*>* nestedRules = getNestedRules();
  list<LessRuleset*>::iterator r_it;

  map<string, TokenList*>::iterator e_find;

  for (e_it = getLessSelector()->getExtensions()->begin();
       e_it != getLessSelector()->getExtensions()->end();
       e_it++) {
    selector = new Selector();
    selector->push(e_it->second);
    
    if (prefix != NULL)
      selector->addPrefix(prefix);

    e_find = extensions->find(e_it->first);
    if (e_find != extensions->end()) {
      e_find->second->push(new Token(",", Token::OTHER));
      e_find->second->push(selector);
      delete selector;
    } else {
      extensions->insert(pair<string, TokenList*>
                         (e_it->first,selector));
    }
  }

  selector = new Selector();
  selector->push(getSelector());
  if (prefix != NULL)
    selector->addPrefix(prefix);
  
  // look through statements for extensions
  for(s_it = getUnprocessedStatements()->begin();
      s_it != getUnprocessedStatements()->end();
      s_it++) {
    extension = (*s_it)->getExtension();
    
    if (extension != NULL) {
      e_find = extensions->find(extension->toString());
      
      if (e_find != extensions->end()) {
        e_find->second->push(new Token(",", Token::OTHER));
        e_find->second->push(selector);
      } else {
        extensions->insert(pair<string, TokenList*>
                           (extension->toString(),
                            selector->clone()));
      }
      delete extension;
    }
  }

  // look in nested rules
  for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
    (*r_it)->getExtensions(extensions, selector);
  }
  delete selector;
}

bool LessRuleset::insert(ParameterMixin* mixin, Ruleset* target) {
  map<string, TokenList*> scope;
  bool ret = false;
  
  getLessStylesheet()->getValueProcessor()->pushScope(&scope);

  if (((mixin == NULL && !selector->needsArguments()) ||
       putArguments(mixin)) &&
      matchConditions()) {

#ifdef WITH_LIBGLOG
    VLOG(2) << "Inserting variables";
#endif
    
    // set local variables
    processVariables();

#ifdef WITH_LIBGLOG
    VLOG(2) << "Inserting statements";
#endif
    
    // process statements
    Ruleset::insert(target);

#ifdef WITH_LIBGLOG
    VLOG(2) << "Inserting nested rules";
#endif
    
    // insert nested rules
    insertNestedRules(target->getStylesheet(), target->getSelector());

    ret = true;
  }
  getLessStylesheet()->getValueProcessor()->popScope();
  return ret;
}

bool LessRuleset::insert(ParameterMixin* mixin, Stylesheet* s) {
  map<string, TokenList*> scope;
  list<UnprocessedStatement*>* unprocessedStatements = getUnprocessedStatements();
  list<UnprocessedStatement*>::iterator up_it;
  bool ret = false;
  
  getLessStylesheet()->getValueProcessor()->pushScope(&scope);

  if (((mixin == NULL && !selector->needsArguments()) ||
       putArguments(mixin)) &&
      matchConditions()) {

    // set local variables
    processVariables();

    // insert mixins
    for (up_it = unprocessedStatements->begin();
         up_it != unprocessedStatements->end();
         up_it++) {
      (*up_it)->insert(s);
    }
    // insert nested rules
    insertNestedRules(s, NULL);
    ret = true;
  }
  getLessStylesheet()->getValueProcessor()->popScope();
  return ret;
}

void LessRuleset::process(Stylesheet* s) {
  process(s, NULL);
}
void LessRuleset::process(Stylesheet* s, Selector* prefix) {
  Ruleset* target;

  if (selector->needsArguments())
    return;
  
  target = new Ruleset();
  target->setSelector(getSelector()->clone());
  if (prefix != NULL)
    target->getSelector()->addPrefix(prefix);

#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing Less Ruleset: " <<
    getSelector()->toString();
#endif

  getLessStylesheet()->getValueProcessor()->interpolateTokenList(target->getSelector());

  s->addStatement(target);
  insert(NULL, target);
}

void LessRuleset::getLessRulesets(list<LessRuleset*>* rulesetList,
                                  ParameterMixin* mixin,
                                  size_t selector_offset) {
  list<LessRuleset*>* nestedRules = getNestedRules();
  list<LessRuleset*>::iterator r_it;

  selector_offset = mixin->name->walk(getSelector(), selector_offset);

  if (selector_offset == Selector::npos)
    return;

#ifdef WITH_LIBGLOG
  VLOG(3) << "Matching mixin " << mixin->name->toString() <<
    " against " << getSelector()->toString();
#endif
  
  while (selector_offset < mixin->name->size() &&
         (mixin->name->at(selector_offset)->type ==
          Token::WHITESPACE ||
          mixin->name->at(selector_offset)->str == ">")) {
    selector_offset++;
  }
#ifdef WITH_LIBGLOG
  VLOG(3) << "Selector offset:" << selector_offset;
#endif

  if (selector_offset == mixin->name->size()) {
    if (selector->matchArguments(mixin))
      rulesetList->push_back(this);

  } else {   
    for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
      (*r_it)->getLessRulesets(rulesetList, mixin, selector_offset);
    }
  }
}

void LessRuleset::getLocalLessRulesets(list<LessRuleset*>* rulesetList,
                                       ParameterMixin* mixin) {
  list<LessRuleset*>* nestedRules = getNestedRules();
  list<LessRuleset*>::iterator r_it;
  
  for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
    (*r_it)->getLessRulesets(rulesetList, mixin, 0);
  }

  if (getParent() != NULL) {
    getParent()->getLocalLessRulesets(rulesetList, mixin);
  } else {
    getLessStylesheet()->getLessRulesets(rulesetList, mixin);
  }
}

void LessRuleset::processVariables() {
  map<string, TokenList*>::iterator it;

  for (it = variables.begin(); it != variables.end(); ++it) {
    getLessStylesheet()->getValueProcessor()->putVariable(it->first, it->second);
  }
}

void LessRuleset::insertNestedRules(Stylesheet* s, Selector* prefix) {
  list<LessRuleset*>* nestedRules = getNestedRules();
  list<LessRuleset*>::iterator r_it;

  for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
    (*r_it)->process(s, prefix);
  }
}


bool LessRuleset::matchConditions(){
  list<TokenList*>* conditions = selector->getConditions();
  list<TokenList*>::iterator cit;
  TokenList* condition;

  if (conditions->size() == 0)
    return true;


  for(cit = conditions->begin(); cit != conditions->end(); cit++) {
    condition = (*cit)->clone();
#ifdef WITH_LIBGLOG
    VLOG(3) << "Checking condition: " << condition->toString();
#endif
    
    if (getLessStylesheet()->getValueProcessor()->validateCondition(condition)) {
      
#ifdef WITH_LIBGLOG
      VLOG(3) << "Found valid condition: " << condition->toString();
#endif
      
      delete condition;
      return true;
    } else
      delete condition;
  }
  return false;
}
  
bool LessRuleset::putArguments(ParameterMixin* mixin) {
  list<string>* parameters = selector->getParameters();
  list<string>::iterator pit = parameters->begin();
  TokenList* argsCombined = new TokenList();
  TokenList* restVar = NULL;
  TokenList* variable;
  size_t pos = 0;

  if (selector->unlimitedArguments() && selector->getRestIdentifier() != "")
    restVar = new TokenList();
  
  // combine with parameter names and add to local scope
  for(; pit != parameters->end(); pit++) {
    variable = mixin->getArgument(*pit);

    if (variable == NULL) 
      variable = mixin->getArgument(pos++);

    if (variable == NULL)
      variable = selector->getDefault(*pit);

    if (variable == NULL) {
      delete argsCombined;
      return false;
    }
    
    getLessStylesheet()->getValueProcessor()->putVariable(*pit, variable);
    argsCombined->push(variable->clone());
    argsCombined->push(new Token(" ", Token::WHITESPACE));
  }

  argsCombined->trim();

  if (restVar != NULL) {
    while (pos < mixin->getArgumentCount()) {
      restVar->push(mixin->getArgument(pos++));
      restVar->push(new Token(" ", Token::WHITESPACE));
    }
    
    restVar->trim();
    getLessStylesheet()->getValueProcessor()->putVariable(selector->getRestIdentifier(), restVar);
  }
  
  getLessStylesheet()->getValueProcessor()->putVariable("@arguments", argsCombined);
  return true;
}
