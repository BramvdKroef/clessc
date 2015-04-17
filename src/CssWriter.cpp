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
  column = 0;
  sourcemap = NULL;
}

CssWriter::CssWriter(std::ostream &out): out(&out), column(0) {
  sourcemap = NULL;
}
CssWriter::CssWriter(std::ostream &out, SourceMapWriter &sourcemap):
  out(&out), column(0), sourcemap(&sourcemap) {
}

CssWriter::~CssWriter() {
}

unsigned int CssWriter::getColumn() {
  return column;
}

void CssWriter::writeStr(const char* str, size_t len) {
  out->write(str, len);
  column += len;
}
void CssWriter::writeToken(const Token &token) {
  writeStr(token.c_str(),
           token.size());
}
void CssWriter::writeTokenList(const TokenList &tokens) {
  TokenList::const_iterator i = tokens.begin();
  
  for(; i != tokens.end(); i++) {
    writeToken(*i);
  }
}

void CssWriter::writeSelector(const TokenList &selector) {
  TokenList::const_iterator it;

  if (sourcemap != NULL)
    sourcemap->writeMapping(column, selector.front());
  
  for (it = selector.begin(); it != selector.end(); it++) {
    writeToken(*it);
      
    if ((*it) == ",") {
      if (sourcemap != NULL)
        sourcemap->writeMapping(column, selector.front());
    }
  }
}

void CssWriter::writeValue(const TokenList &value) {
  TokenList::const_iterator it = value.begin();
  const Token* t;

  while(it != value.end() && (*it).type == Token::WHITESPACE) {
    it++;
  }
  
  if (sourcemap != NULL)
    sourcemap->writeMapping(column, *it);
  t = &(*it);
  
  for (; it != value.end(); it++) {
    
    if ((*it).source != t->source ||
        (*it).line != t->line) {
      if (sourcemap != NULL)
        sourcemap->writeMapping(column, (*it));
      t = &(*it);
    }

    writeToken(*it);
  }
}

void CssWriter::writeAtRule(const Token &keyword, const TokenList &rule) {
  if (sourcemap != NULL)
    sourcemap->writeMapping(column, keyword);
  
  writeToken(keyword);
  writeStr(" ", 1);

  if (sourcemap != NULL)
    sourcemap->writeMapping(column, rule.front());
  
  writeTokenList(rule);
  
  writeStr(";", 1);
}

void CssWriter::writeRulesetStart(const TokenList &selector) {
  
  writeSelector(selector);

  writeStr("{", 1);
}

void CssWriter::writeRulesetEnd() {
  writeStr("}", 1);
}

void CssWriter::writeDeclaration(const Token &property, const TokenList &value) {
  if (sourcemap != NULL)
    sourcemap->writeMapping(column, property);

  writeToken(property);
  writeStr(":", 1);

  writeValue(value);
}

void CssWriter::writeDeclarationDeliminator() {
  writeStr(";", 1);
}

void CssWriter::writeMediaQueryStart(const TokenList &selector) {
  writeSelector(selector);
  writeStr("{", 1);
}

void CssWriter::writeMediaQueryEnd() {
  writeStr("}", 1);
}

void CssWriter::writeSourceMapUrl(const char* sourcemap_url) {
  *out << std::endl << "/*# sourceMappingURL=" <<
    sourcemap_url << "*/" << std::endl;
}
