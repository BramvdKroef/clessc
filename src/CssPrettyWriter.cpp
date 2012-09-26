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


void CssPrettyWriter::writeAtRule(AtRule* atrule) {
  CssWriter::writeAtRule(atrule);
  out->write("\n", 1);
}

void CssPrettyWriter::writeRuleset(Ruleset* ruleset) {
  TokenList* selector = ruleset->getSelector();
  TokenListIterator* it;
  
  vector<Declaration*>* declarations = ruleset->getDeclarations();
  vector<Declaration*>::iterator dIt;

  if (declarations->size() == 0)
    return;
  
  if (selector != NULL) {
    for (it = selector->iterator(); it->hasNext();) {
      Token* next = it->next();
      out->write(next->str.c_str(), next->str.size());
    }
  }
  out->write("{\n", 2);
  for (dIt = declarations->begin(); dIt < declarations->end(); dIt++) {
    out->write("    ", 4);
    writeDeclaration(*dIt);
    out->write(";\n", 2);
  }
  out->write("}\n", 2);
}
