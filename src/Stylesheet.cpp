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

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

void RulesetStatement::setRuleset(Ruleset* r) {
  ruleset = r;
}
Ruleset* RulesetStatement::getRuleset() {
  return ruleset;
}

Declaration::Declaration() {
  property = NULL;
  value = NULL;
}

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
void Declaration::process(Ruleset* r) {
  DLOG(INFO) << "Processing declaration " << *property << ": " <<
    *value->toString();
  r->addStatement(this->clone());
}
void Declaration::write(CssWriter* writer) {
  writer->writeDeclaration(*property, value);
}

void StylesheetStatement::setStylesheet(Stylesheet* s) {
  stylesheet = s;
}
Stylesheet* StylesheetStatement::getStylesheet() {
  return stylesheet;
}


Ruleset::Ruleset() {
}
Ruleset::Ruleset(Selector* selector){
  this->selector = selector;
}

Ruleset::~Ruleset() {
  if (selector != NULL) {
    delete selector;
  }
  clearStatements();
}

void Ruleset::setSelector (Selector* selector) {
  this->selector = selector;
}
void Ruleset::addStatement (RulesetStatement* statement) {
  statements.push_back(statement);
  statement->setRuleset(this);
}
void Ruleset::addStatement(Declaration* declaration) {
  declarations.push_back(declaration);
  statements.push_back(declaration);
  declaration->setRuleset(this);
}

void Ruleset::addStatements (vector<RulesetStatement*>* statements) {
  vector<RulesetStatement*>::iterator i;
  for (i = statements->begin(); i < statements->end(); i++) {
    addStatement(*i);
  }
}
void Ruleset::addDeclarations (vector<Declaration*>* declarations) {
  vector<Declaration*>::iterator i;
  for (i = declarations->begin(); i < declarations->end(); i++) {
    addStatement(*i);
  }
}

Selector* Ruleset::getSelector() {
  return selector;
}
vector<RulesetStatement*>* Ruleset::getStatements() {
  return &statements;
}

vector<Declaration*>* Ruleset::getDeclarations() {
  return &declarations;
}

void Ruleset::clearStatements() {
  while (!statements.empty()) {
    delete statements.back();
    statements.pop_back();
  }
  declarations.clear();
}

vector<Declaration*>* Ruleset::cloneDeclarations() {
  vector<Declaration*>* declarations;
  vector<Declaration*>::iterator i;
  vector<Declaration*>* clone = new vector<Declaration*>();
  
  declarations = getDeclarations();  
  for (i = declarations->begin(); i != declarations->end(); i++) {
    clone->push_back((*i)->clone());
  }
  return clone;
}

Ruleset* Ruleset::clone() {
  Ruleset* ruleset = new Ruleset(NULL);
  vector<RulesetStatement*>* statements;
  vector<RulesetStatement*>::iterator i;
  
  if (getSelector() != NULL)
    ruleset->setSelector(getSelector()->clone());

  statements = getStatements();  
  for (i = statements->begin(); i != statements->end(); i++) {
    ruleset->addStatement((*i)->clone());
  }
  return ruleset;
}

void Ruleset::swap(Ruleset* ruleset) {
  Selector* selector = this->selector;
  this->selector = ruleset->getSelector();
  ruleset->setSelector(selector);
  this->statements.swap(*ruleset->getStatements());
  this->declarations.swap(*ruleset->getDeclarations());
}

void Ruleset::insert(Ruleset* target) {
  vector<RulesetStatement*>* statements = getStatements();
  vector<RulesetStatement*>::iterator i;
  for (i = statements->begin(); i != statements->end(); i++) {
    (*i)->process(target);
  }
}

void Ruleset::process(Stylesheet* s) {
  Ruleset* target = new Ruleset();
  DLOG(INFO) << "Processing Ruleset: " << *getSelector()->toString();
  target->setSelector(getSelector()->clone());
  s->addStatement(target);
  insert(target);
}

void Ruleset::write(CssWriter* writer) {
  vector<RulesetStatement*>* statements;
  vector<RulesetStatement*>::iterator i;

  if (getStatements()->empty())
    return;
  
  writer->writeRulesetStart(selector);
    
  statements = getStatements();  
  for (i = statements->begin(); i != statements->end(); i++) {
    (*i)->write(writer);
    if (i + 1 != statements->end())
      writer->writeDeclarationDeliminator();
  }
  writer->writeRulesetEnd();
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
void AtRule::process(Stylesheet* s) {
  AtRule* target = new AtRule(new string(*keyword));
  target->setRule(rule->clone());

  DLOG(INFO) << "Processing @rule " << *this->getKeyword() << ": " <<
    *this->getRule()->toString();

  s->addStatement(target);
}

void AtRule::write(CssWriter* writer) {
  writer->writeAtRule(*keyword, rule);
}
  
Stylesheet::~Stylesheet() {
  while (!statements.empty()) {
    delete statements.back();
    statements.pop_back();
  }
}

void Stylesheet::addStatement(StylesheetStatement* statement) {
  statements.push_back(statement);
  statement->setStylesheet(this);
  DLOG(INFO) << "Adding statement";
}

void Stylesheet::addStatement(Ruleset* ruleset) {
  DLOG(INFO) << "Adding ruleset: " <<
    *ruleset->getSelector()->toString();

  addStatement((StylesheetStatement*)ruleset);
  rulesets.push_back(ruleset);
}

void Stylesheet::addStatement(AtRule* atrule) {
  DLOG(INFO) << "Adding @rule: " << 
    *atrule->getKeyword() << ": " <<
    *atrule->getRule()->toString();

  addStatement((StylesheetStatement*)atrule);
  atrules.push_back(atrule);
}


vector<AtRule*>* Stylesheet::getAtRules() {
  return &atrules;
}
vector<Ruleset*>* Stylesheet::getRulesets() {
  return &rulesets;
}
vector<StylesheetStatement*>* Stylesheet::getStatements() {
  return &statements;
}

Ruleset* Stylesheet::getRuleset(Selector* selector) {
  vector<Ruleset*>::iterator it;

  for (it = rulesets.begin(); it != rulesets.end(); it++) {
    if ((*it)->getSelector()->equals(selector)) 
      return *it;
  }
  return NULL;
}

void Stylesheet::process(Stylesheet* s) {
  vector<StylesheetStatement*>* statements;
  vector<StylesheetStatement*>::iterator i;
  VLOG(2) << "Processing stylesheet";
  
  statements = getStatements();  
  for (i = statements->begin(); i != statements->end(); i++) {
    (*i)->process(s);
  }
  VLOG(2) << "Done processing stylesheet";
}

void Stylesheet::write(CssWriter* writer) {
  vector<StylesheetStatement*>* statements;
  vector<StylesheetStatement*>::iterator i;
  
  statements = getStatements();  
  for (i = statements->begin(); i != statements->end(); i++) {
    (*i)->write(writer);
  }
}

Selector* MediaQuery::getSelector() {
  return selector;
}
void MediaQuery::setSelector(Selector* s) {
  selector = s;
}

void MediaQuery::process(Stylesheet* s) {
  MediaQuery* query = new MediaQuery();
  DLOG(INFO) << "Processing media query " << *getSelector()->toString();
  query->setSelector(getSelector()->clone());
  s->addStatement(query);
  
  Stylesheet::process(query);
}

void MediaQuery::write(CssWriter* writer) {
  writer->writeMediaQueryStart(selector);
  Stylesheet::write(writer);
  
  writer->writeMediaQueryEnd();
}
