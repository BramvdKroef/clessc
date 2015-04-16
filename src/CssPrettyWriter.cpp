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
    writeStr("  ", 2);
}

void CssPrettyWriter::newline() {
  out->write("\n", 1);
  column = 0;
  
  if (sourcemap != NULL)
    sourcemap->writeNewline();
}

void CssPrettyWriter::writeSelector(const TokenList &selector) {
  TokenList::const_iterator it;

  if (sourcemap != NULL)
    sourcemap->writeMapping(column, selector.front());
  
  for (it = selector.begin(); it != selector.end(); it++) {
    writeToken(*it);
      
    if ((*it) == ",") {
      if (sourcemap != NULL)
        sourcemap->writeMapping(column, selector.front());

      newline();
      indent();
    }
  }
}

void CssPrettyWriter::writeAtRule(const Token &keyword, const TokenList &rule) {
  indent();
  
  CssWriter::writeAtRule(keyword, rule);
  newline();
}

void CssPrettyWriter::writeRulesetStart(const TokenList &selector) {
  indent();
  
  writeSelector(selector);
  
  writeStr(" {", 2);
  newline();
  indent_size++;
}

void CssPrettyWriter::writeRulesetEnd() {
  writeStr(";", 1);
  newline();
  
  indent_size--;
  indent();

  writeStr("}", 1);
  newline();
}
void CssPrettyWriter::writeDeclaration(const Token &property,
                                       const TokenList &value) {

  indent();
  
  if (sourcemap != NULL)
    sourcemap->writeMapping(column, property);

  writeToken(property);
  writeStr(": ", 2);

  writeValue(value);
}

void CssPrettyWriter::writeDeclarationDeliminator() {
  writeStr(";", 1);
  newline();
}

void CssPrettyWriter::writeMediaQueryStart(const TokenList &selector) {
  indent();
  
  writeSelector(selector);
  
  writeStr(" {", 2);
  newline();
  indent_size++;
}

void CssPrettyWriter::writeMediaQueryEnd() {
  writeStr("}", 1);
  indent_size--;
  newline();
}
