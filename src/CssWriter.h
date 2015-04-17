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

#ifndef __CssWriter_h__
#define __CssWriter_h__

#include "TokenList.h"
#include "SourceMapWriter.h"
#include <iostream>

class CssWriter {
protected:
  std::ostream* out;
  unsigned int column;
  SourceMapWriter* sourcemap;

  void writeStr(const char* str, size_t len);
  void writeToken(const Token &token);
  void writeTokenList(const TokenList &tokens);
  
  virtual void writeSelector(const TokenList &selector);
  virtual void writeValue(const TokenList &value);
  
public:
  CssWriter();
  CssWriter(std::ostream &out);
  CssWriter(std::ostream &out,
            SourceMapWriter &sourcemap);

  unsigned int getColumn();
  
  virtual ~CssWriter();
  virtual void writeAtRule(const Token &keyword, const TokenList &rule);
  virtual void writeRulesetStart(const TokenList &selector);
  virtual void writeRulesetEnd();
  virtual void writeDeclaration(const Token &property, const TokenList &value);
  virtual void writeDeclarationDeliminator();
  virtual void writeMediaQueryStart(const TokenList &selector);
  virtual void writeMediaQueryEnd();

  void writeSourceMapUrl(const char* sourcemap_url);
};
  
#endif
