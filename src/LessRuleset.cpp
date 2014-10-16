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
    *nestedRule->getSelector()->toString();
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

void LessRuleset::getExtensions(map<string, TokenList*>* extensions) {
  map<string, TokenList*>::iterator e_it;
  
  list<UnprocessedStatement*>::iterator s_it;
  TokenList* extension;

  for (e_it = selector->getExtensions()->begin();
       e_it != selector->getExtensions()->end();
       e_it++) {
    extensions->insert(pair<string, TokenList*>
                       (e_it->first,
                        e_it->second->clone()));
  }
  
  // look through statements for extensions
  for(s_it = getUnprocessedStatements()->begin();
      s_it != getUnprocessedStatements()->end();
      s_it++) {
    extension = (*s_it)->getExtension();
    
    if (extension != NULL) {
      extensions->insert(pair<string, TokenList*>
                         (*extension->toString(),
                          getSelector()->clone()));
      delete extension;
    }
  }
}

bool LessRuleset::insert(list<TokenList*>* arguments, Ruleset* target) {
  map<string, TokenList*> scope;
  bool ret = false;
  
  getLessStylesheet()->getValueProcessor()->pushScope(&scope);

  if (((arguments == NULL && !selector->needsArguments()) ||
       putArguments(arguments)) &&
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

bool LessRuleset::insert(list<TokenList*>* arguments, Stylesheet* s) {
  map<string, TokenList*> scope;
  list<UnprocessedStatement*>* unprocessedStatements = getUnprocessedStatements();
  list<UnprocessedStatement*>::iterator up_it;
  bool ret = false;
  
  getLessStylesheet()->getValueProcessor()->pushScope(&scope);

  if (((arguments == NULL && !selector->needsArguments()) ||
       putArguments(arguments)) &&
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
    *getSelector()->toString();
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

  if (selector_offset == 0)
    return;

#ifdef WITH_LIBGLOG
  VLOG(3) << "Matching mixin " << *mixin->name->toString() <<
    " against " << *getSelector()->toString();
#endif
  
  while (selector_offset < mixin->name->size() &&
         (mixin->name->at(selector_offset)->type ==
          Token::WHITESPACE ||
          mixin->name->at(selector_offset)->str == ">")) {
    selector_offset++;
  }
#ifdef WITH_LIBGLOG
  VLOG(3) "Selector offset:" << selector_offset;
#endif

  if (selector_offset == mixin->name->size()) {
    if (selector->matchArguments(mixin->arguments))
      rulesetList->push_back(this);

  } else {   
    for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
      (*r_it)->getLessRulesets(rulesetList, mixin, selector_offset);
    }
  }
}

void LessRuleset::processVariables() {
  map<string, TokenList*>::iterator it;

  for (it = variables.begin(); it != variables.end(); ++it) {
    getLessStylesheet()->putVariable(it->first, it->second);
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
    VLOG(3) << "Checking condition: " << *condition->toString();
#endif
    
    if (getLessStylesheet()->getValueProcessor()->validateValue(condition)) {

#ifdef WITH_LIBGLOG
      VLOG(3) << "Found valid condition: " << *condition->toString();
#endif
      
      delete condition;
      return true;
    } else
      delete condition;
  }
  return false;
}
  
bool LessRuleset::putArguments(list<TokenList*>* arguments) {
  list<string>* parameters = selector->getParameters();
  list<TokenList*>::iterator ait  = arguments->begin();
  list<string>::iterator pit = parameters->begin();
  TokenList* argsCombined = new TokenList();
  TokenList* restVar = NULL;
  TokenList* variable;

  if (selector->unlimitedArguments() && selector->getRestIdentifier() != "")
    restVar = new TokenList();
  
  // combine with parameter names and add to local scope
  for(; pit != parameters->end(); pit++) {
    if (ait != arguments->end()) {
      getLessStylesheet()->putVariable(*pit, *ait);
      argsCombined->push((*ait)->clone());
      ait++;
    } else {
      variable = selector->getDefault(*pit);
      if (variable == NULL) {
        delete argsCombined;
        return false;
      }
      getLessStylesheet()->putVariable(*pit, variable->clone());
      argsCombined->push(variable->clone());
    }
    argsCombined->push(new Token(" ", Token::WHITESPACE));
  }

  argsCombined->trim();

  if (restVar != NULL) {
    while (ait != arguments->end()) {
      restVar->push(*ait);
      restVar->push(new Token(" ", Token::WHITESPACE));
      ait++;
    }
    restVar->trim();
    getLessStylesheet()->putVariable(selector->getRestIdentifier(), restVar);
  }
  
  getLessStylesheet()->putVariable("@arguments", argsCombined);
  return true;
}
