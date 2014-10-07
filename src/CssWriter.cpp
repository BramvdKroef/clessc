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


void CssWriter::writeAtRule(string keyword, TokenList* rule) {
  TokenListIterator* it = rule->iterator();
  
  out->write(keyword.c_str(),
             keyword.size());
  out->write(" ", 1);
  
  while (it->hasNext()) {
    Token* next = it->next();
    out->write(next->str.c_str(), next->str.size());
  }
  out->write(";", 1);
}

void CssWriter::writeRulesetStart(TokenList* selector) {
  TokenListIterator* it;
  
  if (selector != NULL) {
    for (it = selector->iterator(); it->hasNext();) {
      Token* next = it->next();
      out->write(next->str.c_str(), next->str.size());
    }
  }
  out->write("{", 1);
}

void CssWriter::writeRulesetEnd() {
  out->write("}", 1);
}

void CssWriter::writeDeclaration(string property, TokenList* value) {
  TokenListIterator* it = value->iterator();
    
  out->write(property.c_str(), property.size());
  out->write(":", 1);
  
  while (it->hasNext()) {
    Token* next = it->next();
    out->write(next->str.c_str(), next->str.size());
  }
}

void CssWriter::writeDeclarationDeliminator() {
  out->write(";", 1);
}

void CssWriter::writeMediaQueryStart(TokenList* selector) {
  TokenListIterator* it;
  
  if (selector != NULL) {
    for (it = selector->iterator(); it->hasNext();) {
      Token* next = it->next();
      out->write(next->str.c_str(), next->str.size());
    }
  }
  out->write("{", 1);
}

void CssWriter::writeMediaQueryEnd() {
  out->write("}", 1);
}
