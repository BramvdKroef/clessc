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

CssWriter::CssWriter() {
  out = NULL;
}

CssWriter::CssWriter(ostream &out) {
  this->out = &out;
}

CssWriter::~CssWriter() {
}


void CssWriter::writeAtRule(const string &keyword, const TokenList &rule) {
  TokenList::const_iterator i = rule.begin();
  
  out->write(keyword.c_str(),
             keyword.size());
  out->write(" ", 1);
  
  for(; i != rule.end(); i++) {
    const Token next = *i;
    out->write(next.c_str(), next.size());
  }
  out->write(";", 1);
}

void CssWriter::writeRulesetStart(const TokenList &selector) {
  TokenList::const_iterator i;
  
  for (i = selector.begin(); i != selector.end(); i++) {
    const Token next = *i;
    out->write(next.c_str(), next.size());
  }
  out->write("{", 1);
}

void CssWriter::writeRulesetEnd() {
  out->write("}", 1);
}

void CssWriter::writeDeclaration(const string &property, const TokenList &value) {
  TokenList::const_iterator i;
    
  out->write(property.c_str(), property.size());
  out->write(":", 1);
  
  for (i = value.begin(); i != value.end(); i++) {
    const Token next = *i;
    out->write(next.c_str(), next.size());
  }
}

void CssWriter::writeDeclarationDeliminator() {
  out->write(";", 1);
}

void CssWriter::writeMediaQueryStart(const TokenList &selector) {
  TokenList::const_iterator i;
  
  for (i = selector.begin(); i != selector.end(); i++) {
    const Token next = *i;
    out->write(next.c_str(), next.size());
  }
  out->write("{", 1);
}

void CssWriter::writeMediaQueryEnd() {
  out->write("}", 1);
}
