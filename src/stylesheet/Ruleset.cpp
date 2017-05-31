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

#include "Ruleset.h"

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
Declaration* Ruleset::createDeclaration(const Token &property) {
  Declaration* d = new Declaration(property);
  declarations.push_back(d);
  addStatement(*d);
  return d;
}

CssComment* Ruleset::createComment() {
  CssComment* c = new CssComment();
  addStatement(*c);
  return c;
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
const std::list<RulesetStatement*>& Ruleset::getStatements() const {
  return statements;
}

std::list<Declaration*>& Ruleset::getDeclarations() {
  return declarations;
}

void Ruleset::clearStatements() {
  declarations.clear();
  while(!statements.empty()) {
    delete statements.back();
    statements.pop_back();
  }
}

void Ruleset::insert(Ruleset &target) const {
  std::list<RulesetStatement*> statements = getStatements();
  std::list<RulesetStatement*>::iterator i;
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
  std::list<RulesetStatement*> statements = getStatements();
  std::list<RulesetStatement*>::iterator i;

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
