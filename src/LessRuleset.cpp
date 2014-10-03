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
}
LessRuleset::LessRuleset(Selector* selector) : Ruleset(selector) {
  parent = NULL;
  lessStylesheet = NULL;
}
LessRuleset::~LessRuleset() {
  while (!nestedRules.empty()) {
    delete nestedRules.back();
    nestedRules.pop_back();
  }
}
  
void LessRuleset::addStatement(UnprocessedStatement* statement) {
  Ruleset::addStatement(statement);
  statement->setRuleset(this);
}

void LessRuleset::addNestedRule(LessRuleset* nestedRule) {
  DLOG(INFO) << "Adding nested rule: " << *nestedRule->getSelector()->toString();
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
  DLOG(INFO) << "set LessStylesheet";
  lessStylesheet = s;
  Ruleset::setStylesheet(s);
}

LessStylesheet* LessRuleset::getLessStylesheet() {
  return lessStylesheet;
}

void LessRuleset::insert(Ruleset* target) {
  map<string, TokenList*> scope;

  getLessStylesheet()->getValueProcessor()->pushScope(&scope);

  DLOG(INFO) << "Inserting variables";
  // set local variables
  processVariables();

  DLOG(INFO) << "Inserting statements";
  // process statements
  Ruleset::insert(target);

  DLOG(INFO) << "Inserting nested rules";
  // insert nested rules
  insertNestedRules(target->getStylesheet(), target->getSelector());
  
  getLessStylesheet()->getValueProcessor()->popScope();
}

void LessRuleset::insert(Stylesheet* s) {
  map<string, TokenList*> scope;  
  getLessStylesheet()->getValueProcessor()->pushScope(&scope);
  // set local variables
  processVariables();
  // insert nested rules
  insertNestedRules(s, NULL);
  
  getLessStylesheet()->getValueProcessor()->popScope();
}

void LessRuleset::process(Stylesheet* s) {
  Ruleset* target = new Ruleset();
  DLOG(INFO) << "Processing Less Ruleset: " << *getSelector()->toString();
  target->setSelector(getSelector()->clone());

  DLOG(INFO) << "Interpolating selector " << *target->getSelector()->toString();
  getLessStylesheet()->getValueProcessor()->interpolateTokenList(target->getSelector());

  s->addStatement(target);
  insert(target);
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
  LessRuleset* nestedRule;
  Selector* oldSelector;

  for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
    nestedRule = *r_it;

    if (prefix != NULL) {
      oldSelector = nestedRule->getSelector()->clone();
      nestedRule->getSelector()->addPrefix(prefix);
    }

    nestedRule->process(s);
    DLOG(INFO) << "Processed nested rule";
    if (prefix != NULL) {
      delete nestedRule->getSelector();
      nestedRule->setSelector(oldSelector);
    }
  }
}

