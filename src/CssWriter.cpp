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

#include "CssWriter.h"

CssWriter::CssWriter(ostream* out) {
  this->out = out;
}

CssWriter::~CssWriter() {
}

void CssWriter::writeStylesheet(Stylesheet* s) {
  vector<AtRule*>* atrules = s->getAtRules();
  vector<AtRule*>::iterator aIt;
  vector<Ruleset*>* rulesets = s->getRulesets();
  vector<Ruleset*>::iterator rIt;

  for (aIt = atrules->begin(); aIt < atrules->end(); aIt++) {
    writeAtRule(*aIt);
  }
  for (rIt = rulesets->begin(); rIt < rulesets->end(); rIt++) {
    writeRuleset(*rIt);
  }
}

void CssWriter::writeAtRule(AtRule* atrule) {
  TokenList* rule = atrule->getRule();
  TokenListIterator* it = rule->iterator();
  
  out->write(atrule->getKeyword()->c_str(),
             atrule->getKeyword()->size());
  out->write(" ", 1);
  
  while (it->hasNext()) {
    Token* next = it->next();
    out->write(next->str.c_str(), next->str.size());
  }
  out->write(";", 1);
}

void CssWriter::writeRuleset(Ruleset* ruleset) {
  TokenList* selector = ruleset->getSelector();
  TokenListIterator* it;
  
  vector<Declaration*>* declarations = ruleset->getDeclarations();
  vector<Declaration*>::iterator dIt;

  if (declarations->size() == 0)
    return;
  
  if (selector != NULL) {
    for (it = selector->iterator(); it->hasNext();) {
      Token* next = it->next();
      out->write(next->str.c_str(), next->str.size());
    }
  }
  out->write("{", 1);
  for (dIt = declarations->begin(); dIt < declarations->end(); dIt++) {
    writeDeclaration(*dIt);
    if (dIt + 1 < declarations->end())
      out->write(";", 1);
  }
  out->write("}", 1);
}

void CssWriter::writeDeclaration(Declaration* declaration) {
  TokenList* value = declaration->getValue();
  TokenListIterator* it = value->iterator();
    
  out->write(declaration->getProperty()->c_str(),
             declaration->getProperty()->size());
  out->write(":", 1);
  
  while (it->hasNext()) {
    Token* next = it->next();
    out->write(next->str.c_str(), next->str.size());
  }
}

