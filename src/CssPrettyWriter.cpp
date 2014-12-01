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

#include "CssPrettyWriter.h"

void CssPrettyWriter::indent() {
  int i;
  if (indent_size == 0)
    return;
  for (i = 0; i < indent_size; i++)
    out->write("  ", 2);
}

void CssPrettyWriter::writeAtRule(const string &keyword, const TokenList &rule) {
  indent();
  
  CssWriter::writeAtRule(keyword, rule);
  out->write("\n", 1);
}

void CssPrettyWriter::writeRulesetStart(const TokenList &selector) {
  TokenList::const_iterator it;
  
  indent();
  
  for (it = selector.begin(); it != selector.end(); it++) {
    out->write((*it).c_str(), (*it).size());
      
    if ((*it) == ",") {
      out->write("\n", 1);
      indent();
    }
  }
  
  out->write(" {\n", 3);
  indent_size++;
}

void CssPrettyWriter::writeRulesetEnd() {
  out->write(";\n", 2);
  indent_size--;
  indent();
  out->write("}\n", 2);
}
void CssPrettyWriter::writeDeclaration(const string &property,
                                       const TokenList &value) {
  TokenList::const_iterator it;

  indent();
    
  out->write(property.c_str(), property.size());
  out->write(": ", 2);

  it = value.begin();

  while(it != value.end() &&
        (*it).type == Token::WHITESPACE) {
    it++;
  }
  
  for (; it != value.end(); it++) {
    out->write((*it).c_str(), (*it).size());
  }
}
void CssPrettyWriter::writeDeclarationDeliminator() {
  out->write(";\n", 2);  
}

void CssPrettyWriter::writeMediaQueryStart(const TokenList &selector) {
  TokenList::const_iterator it;
  
  for (it = selector.begin(); it != selector.end(); it++) {
    out->write((*it).c_str(), (*it).size());
  }
  
  out->write(" {\n", 3);
  indent_size++;
}

void CssPrettyWriter::writeMediaQueryEnd() {
  indent_size--;
  out->write("}\n", 2);
}
