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
  property = "";
}

Declaration::Declaration(const string &property) {
  this->property = property;
}

Declaration::~Declaration() {
}

void Declaration::setProperty(const string &property) {
  this->property = property;
}
void Declaration::setValue(const TokenList &value) {
  this->value = value;
}
std::string Declaration::getProperty() {
  return property;
}
TokenList& Declaration::getValue() {
  return value;
}

void Declaration::process(Ruleset &r) {
#ifdef WITH_LIBGLOG
    VLOG(2) << "Processing declaration " << property << ": " <<
      value.toString();
#endif
    Declaration* d = r.createDeclaration();
    d->setProperty(property);
    d->setValue(value);
}


void Declaration::write(CssWriter &writer) {
  writer.writeDeclaration(property, value);
}

void StylesheetStatement::setStylesheet(Stylesheet* s) {
  stylesheet = s;
}
Stylesheet* StylesheetStatement::getStylesheet() {
  return stylesheet;
}


Ruleset::Ruleset() {
}
Ruleset::Ruleset(const Selector &selector){
  this->selector = selector;
}

Ruleset::~Ruleset() {
  clearStatements();
}

void Ruleset::setSelector (const Selector &selector) {
  this->selector = selector;
}
void Ruleset::addStatement (RulesetStatement &statement) {
  statements.push_back(&statement);
  statement.setRuleset(this);
}
Declaration* Ruleset::createDeclaration() {
  Declaration* d = new Declaration();
  declarations.push_back(d);
  addStatement(*d);
  return d;
}
Declaration* Ruleset::createDeclaration(const std::string &property) {
  Declaration* d = new Declaration(property);
  declarations.push_back(d);
  addStatement(*d);
  return d;
}

void Ruleset::deleteStatement(RulesetStatement &statement) {
  statements.remove(&statement);
  delete &statement;
}

void Ruleset::deleteDeclaration(Declaration &declaration) {
  declarations.remove(&declaration);
  deleteStatement(declaration);
}

void Ruleset::addDeclarations (std::list<Declaration> &declarations) {
  std::list<Declaration>::iterator i = declarations.begin();
  for (; i != declarations.end(); i++) {
    this->declarations.push_back(&(*i));
    addStatement(*i);
  }
}

Selector& Ruleset::getSelector() {
  return selector;
}
const Selector& Ruleset::getSelector() const {
  return selector;
}
list<RulesetStatement*>& Ruleset::getStatements() {
  return statements;
}

list<Declaration*>& Ruleset::getDeclarations() {
  return declarations;
}

void Ruleset::clearStatements() {
  declarations.clear();
  while(!statements.empty()) {
    delete statements.back();
    statements.pop_back();
  }
}

void Ruleset::insert(Ruleset &target) {
  list<RulesetStatement*> statements = getStatements();
  list<RulesetStatement*>::iterator i;
  for (i = statements.begin(); i != statements.end(); i++) {
    (*i)->process(target);
  }
}

void Ruleset::process(Stylesheet &s) {
  Ruleset* target = s.createRuleset();
    
#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing Ruleset: " << getSelector().toString();
#endif
        
  target->setSelector(getSelector());
  insert(*target);
}

void Ruleset::write(CssWriter &writer) {
  list<RulesetStatement*> statements = getStatements();
  list<RulesetStatement*>::iterator i;

  if (getStatements().empty())
    return;
  
  writer.writeRulesetStart(getSelector());

  for (i = statements.begin(); i != statements.end(); i++) {
    if (i != statements.begin())
      writer.writeDeclarationDeliminator();
    
    (*i)->write(writer);
  }
  writer.writeRulesetEnd();
}

AtRule::AtRule (const string &keyword) {
  this->keyword = keyword;
}

AtRule::~AtRule() {
}

void AtRule::setKeyword (const string &keyword) {
  this->keyword = keyword;
}
void AtRule::setRule(const TokenList &rule) {
  this->rule = rule;
}
std::string& AtRule::getKeyword() {
  return keyword;
}
TokenList& AtRule::getRule() {
  return rule;
}

void AtRule::process(Stylesheet &s) {
  AtRule* target = s.createAtRule(keyword);
  target->setRule(rule);
      
#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing @rule " << this->getKeyword() << ": " <<
    this->getRule().toString();
#endif
}

void AtRule::write(CssWriter &writer) {
  writer.writeAtRule(keyword, rule);
}
  
Stylesheet::~Stylesheet() {
  rulesets.clear();
  atrules.clear();
  while(!statements.empty()) {
    delete statements.back();
    statements.pop_back();
  }
}

void Stylesheet::addStatement(StylesheetStatement &statement) {
  statements.push_back(&statement);
  statement.setStylesheet(this);
  
#ifdef WITH_LIBGLOG
  VLOG(3) << "Adding statement";
#endif
}
void Stylesheet::addRuleset(Ruleset &ruleset) {
  addStatement(ruleset);
  rulesets.push_back(&ruleset);
}
void Stylesheet::addAtRule(AtRule &rule) {
  addStatement(rule);
  atrules.push_back(&rule);
}


Ruleset* Stylesheet::createRuleset() {
  Ruleset* r = new Ruleset();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating ruleset";
#endif

  addRuleset(*r);
  
  return r;
}

Ruleset* Stylesheet::createRuleset(const Selector &selector) {
  Ruleset* r = new Ruleset(selector);

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating ruleset: " << selector.toString();
#endif

  addRuleset(*r);
  
  return r;
}

AtRule* Stylesheet::createAtRule(const std::string &keyword) {
  AtRule* r = new AtRule(keyword);

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating @rule";
#endif
  
  addStatement(*r);
  atrules.push_back(r);
  return r;
}

MediaQuery* Stylesheet::createMediaQuery() {
  MediaQuery* q = new MediaQuery();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating media query";
#endif
  
  addStatement(*q);
  return q;
}

void Stylesheet::deleteStatement(StylesheetStatement &statement) {
  statements.remove(&statement);
  delete &statement;
}

void Stylesheet::deleteRuleset(Ruleset &ruleset) {
  rulesets.remove(&ruleset);
  deleteStatement(ruleset);
}
void Stylesheet::deleteAtRule(AtRule &atrule) {
  atrules.remove(&atrule);
  deleteStatement(atrule);
}
void Stylesheet::deleteMediaQuery(MediaQuery &query) {
  deleteStatement(query);
}

list<AtRule*>& Stylesheet::getAtRules() {
  return atrules;
}
list<Ruleset*>& Stylesheet::getRulesets() {
  return rulesets;
}
list<StylesheetStatement*>& Stylesheet::getStatements() {
  return statements;
}

Ruleset* Stylesheet::getRuleset(const Selector &selector) {
  list<Ruleset*>::iterator it;

  for (it = rulesets.begin(); it != rulesets.end(); it++) {
    if ((*it)->getSelector().match(selector))
      return *it;
  }
  return NULL;
}

void Stylesheet::process(Stylesheet &s) {
  list<StylesheetStatement*> statements = getStatements();
  list<StylesheetStatement*>::iterator i;
  
#ifdef WITH_LIBGLOG
  VLOG(1) << "Processing stylesheet";
#endif
        
  for (i = statements.begin(); i != statements.end(); i++) {
    (*i)->process(s);
  }
  
#ifdef WITH_LIBGLOG
  VLOG(1) << "Done processing stylesheet";
#endif
}


void Stylesheet::write(CssWriter &writer) {
  list<StylesheetStatement*> statements = getStatements();
  list<StylesheetStatement*>::iterator i;
  
  for (i = statements.begin(); i != statements.end(); i++) {
    (*i)->write(writer);
  }
}

Selector& MediaQuery::getSelector() {
  return selector;
}
void MediaQuery::setSelector(const Selector &s) {
  selector = s;
}

MediaQuery* MediaQuery::createMediaQuery() {
  MediaQuery* q = getStylesheet()->createMediaQuery();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating media query";
#endif

  q->setSelector(getSelector());
  
  return q;
}

void MediaQuery::process(Stylesheet &s) {
  MediaQuery* query = s.createMediaQuery();
    
#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing media query " << getSelector().toString();
#endif
      
  query->setSelector(getSelector());
    
  Stylesheet::process(*query);
}

void MediaQuery::write(CssWriter &writer) {
  writer.writeMediaQueryStart(selector);
  Stylesheet::write(writer);
  
  writer.writeMediaQueryEnd();
}
