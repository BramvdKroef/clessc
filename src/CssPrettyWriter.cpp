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

void CssPrettyWriter::writeAtRule(string keyword, TokenList* rule) {
  indent();
  
  CssWriter::writeAtRule(keyword, rule);
  out->write("\n", 1);
}

void CssPrettyWriter::writeRulesetStart(TokenList* selector) {
  TokenListIterator* it;

  indent();
  
  if (selector != NULL) {
    for (it = selector->iterator(); it->hasNext();) {
      Token* next = it->next();
      out->write(next->str.c_str(), next->str.size());
      
      if (next->str == ",") {
        out->write("\n", 1);
        indent();
      }
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
void CssPrettyWriter::writeDeclaration(string property,
                                       TokenList* value) {
  TokenListIterator* it = value->iterator();

  indent();
    
  out->write(property.c_str(), property.size());
  out->write(": ", 2);

  value->ltrim();
  while (it->hasNext()) {
    Token* next = it->next();
    out->write(next->str.c_str(), next->str.size());
  }
}
void CssPrettyWriter::writeDeclarationDeliminator() {
  out->write(";\n", 2);  
}

void CssPrettyWriter::writeMediaQueryStart(TokenList* selector) {
  TokenListIterator* it;
  
  if (selector != NULL) {
    for (it = selector->iterator(); it->hasNext();) {
      Token* next = it->next();
      out->write(next->str.c_str(), next->str.size());
    }
  }
  out->write(" {\n", 3);
  indent_size++;
}

void CssPrettyWriter::writeMediaQueryEnd() {
  indent_size--;
  out->write("}\n", 2);
}
