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

#include "ExtendCssWriter.h"

ExtendCssWriter::ExtendCssWriter(CssWriter* writer,
                                 map<string,TokenList*>* extensions) {
  this->writer = writer;
  this->extensions = extensions;
}

ExtendCssWriter::~ExtendCssWriter() {
}


void ExtendCssWriter::writeAtRule(string keyword, TokenList* rule) {
  writer->writeAtRule(keyword, rule);
}

void ExtendCssWriter::writeRulesetStart(TokenList* selector) {
  map<string, TokenList*>::iterator mit;
  
  mit = extensions->find(*selector->toString());
  if (mit != extensions->end()) {
    selector->push(new Token(",", Token::OTHER));
    selector->push(mit->second);
  }
  
  writer->writeRulesetStart(selector);
}

void ExtendCssWriter::writeRulesetEnd() {
  writer->writeRulesetEnd();
}

void ExtendCssWriter::writeDeclaration(string property, TokenList* value) {
  writer->writeDeclaration(property, value);
}

void ExtendCssWriter::writeDeclarationDeliminator() {
  writer->writeDeclarationDeliminator();
}

void ExtendCssWriter::writeMediaQueryStart(TokenList* selector) {
  writer->writeMediaQueryStart(selector);
}

void ExtendCssWriter::writeMediaQueryEnd() {
  writer->writeMediaQueryEnd();
}
