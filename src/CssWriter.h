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

#include "Stylesheet.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class CssWriter {
public:
  CssWriter(ostream* out);
  virtual ~CssWriter();
  void writeStylesheet(Stylesheet* s);

protected:
  ostream* out;
  virtual void writeAtRule(AtRule* rule);
  virtual void writeRuleset(Ruleset* ruleset);
  void writeDeclaration(Declaration* declaration);
  
};
  
#endif
