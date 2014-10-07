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


void CssPrettyWriter::writeAtRule(string keyword, TokenList* rule) {
  CssWriter::writeAtRule(keyword, rule);
  out->write("\n", 1);
}

void CssPrettyWriter::writeRulesetStart(TokenList* selector) {
  CssWriter::writeRulesetStart(selector);
  out->write("\n", 1);
}

void CssPrettyWriter::writeRulesetEnd() {
  out->write(";\n}\n", 4);
}
void CssPrettyWriter::writeDeclaration(string property,
                                       TokenList* value) {
  out->write("    ", 4);
  CssWriter::writeDeclaration(property, value);
}
void CssPrettyWriter::writeDeclarationDeliminator() {
  out->write(";\n", 2);  
}

void CssPrettyWriter::writeMediaQueryStart(TokenList* selector) {
  CssWriter::writeMediaQueryStart(selector);
  out->write("\n", 1);
}

void CssPrettyWriter::writeMediaQueryEnd() {
  out->write("}\n", 2);
}
