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

#include "Stylesheet.h"

Declaration::Declaration(string* property) {
  this->property = property;
  value = NULL;
}

Declaration::~Declaration() {
  if (property != NULL)
    delete property;

  if (value != NULL) {
    while (!value->empty()) 
      delete value->pop();
    delete value;
  }
}

void Declaration::setProperty(string* property) {
  this->property = property;
}
void Declaration::setValue(TokenList* value) {
  this->value = value;
}
string* Declaration::getProperty() {
  return property;
}
TokenList* Declaration::getValue() {
  return value;
}
Declaration* Declaration::clone() {
  Declaration* clone =
    new Declaration(new string(*this->getProperty()));
  
  clone->setValue(value->clone());
  return clone;
}

Ruleset::Ruleset() {
}
Ruleset::Ruleset(Selector* selector) {
  this->selector = selector;
}

Ruleset::~Ruleset() {
  if (selector != NULL) {
    delete selector;
  }
  
  while (!declarations.empty()) {
    delete declarations.back();
    declarations.pop_back();
  }
}

void Ruleset::setSelector (Selector* selector) {
  this->selector = selector;
}
void Ruleset::addDeclaration (Declaration* declaration) {
  declarations.push_back(declaration);
}
void Ruleset::addDeclarations (vector<Declaration*>* declarations) {
  vector<Declaration*>::iterator di;
  for (di = declarations->begin(); di < declarations->end(); di++) {
    addDeclaration(*di);
  }
}

Selector* Ruleset::getSelector() {
  return selector;
}
vector<Declaration*>* Ruleset::getDeclarations() {
  return &declarations;
}
vector<Declaration*>* Ruleset::cloneDeclarations() {
  vector<Declaration*>* declarations;
  vector<Declaration*>::iterator di;
  vector<Declaration*>* clone = new vector<Declaration*>();
  
  declarations = getDeclarations();  
  for (di = declarations->begin(); di < declarations->end(); di++) {
    clone->push_back((*di)->clone());
  }
  return clone;
}

Ruleset* Ruleset::clone() {
  Ruleset* ruleset = new Ruleset(NULL);
  vector<Declaration*>* declarations;
  vector<Declaration*>::iterator di;
  
  if (getSelector() != NULL)
    ruleset->setSelector(getSelector()->clone());

  declarations = getDeclarations();  
  for (di = declarations->begin(); di < declarations->end(); di++) {
    ruleset->addDeclaration((*di)->clone());
  }
  return ruleset;
}


AtRule::AtRule (string* keyword) {
  this->keyword = keyword;
  rule = NULL;
}

AtRule::~AtRule() {
  if (keyword != NULL)
    delete keyword;

  if (rule != NULL) 
    delete rule;
}

void AtRule::setKeyword (string* keyword) {
  this->keyword = keyword;
}
void AtRule::setRule(TokenList* rule) {
  this->rule = rule;
}
string* AtRule::getKeyword() {
  return keyword;
}
TokenList* AtRule::getRule() {
  return rule;
}

Stylesheet::~Stylesheet() {
  while (!atrules.empty()) {
    delete atrules.back();
    atrules.pop_back();
  }
  while (!rulesets.empty()) {
    delete rulesets.back();
    rulesets.pop_back();
  }
}

void Stylesheet::addRuleset(Ruleset* ruleset) {
  rulesets.push_back(ruleset);
}

void Stylesheet::addAtRule(AtRule* atrule) {
  atrules.push_back(atrule);
}
vector<AtRule*>* Stylesheet::getAtRules() {
  return &atrules;
}
vector<Ruleset*>* Stylesheet::getRulesets() {
  return &rulesets;
}
Ruleset* Stylesheet::getRuleset(Selector* selector) {
  vector<Ruleset*>::iterator it;
  for (it = rulesets.begin(); it < rulesets.end(); it++) {
    if ((*it)->getSelector()->equals(selector)) 
      return *it;
  }
  return NULL;
}
