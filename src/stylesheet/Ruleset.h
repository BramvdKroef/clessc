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

#ifndef __Ruleset_h__
#define __Ruleset_h__

#include "Stylesheet.h"
#include "StylesheetStatement.h"
#include "Selector.h"
#include "CssComment.h"

#include "../Token.h"
#include "../TokenList.h"

#include "../css/CssWriter.h"

class RulesetStatement;
class Declaration;

class Ruleset: public StylesheetStatement {
private:
  std::list<RulesetStatement*> statements;
  std::list<Declaration*> declarations;

protected:
  Selector selector;
  virtual void addStatement(RulesetStatement &statement);
  virtual void deleteStatement(RulesetStatement &statement);
  
public:
  Ruleset ();
  Ruleset(const Selector &selector);
  virtual ~Ruleset();
  virtual void setSelector (const Selector &selector);

  Declaration* createDeclaration();
  Declaration* createDeclaration(const Token &property);
  CssComment* createComment();
  
  void deleteDeclaration(Declaration &declaration);
  
  void addDeclarations (std::list<Declaration> &declarations);

  Selector& getSelector();
  const Selector& getSelector() const;
  const std::list<RulesetStatement*>& getStatements() const;
  std::list<Declaration*>& getDeclarations();

  void clearStatements();
  
  virtual void insert(Ruleset &target) const;
  virtual void process(Stylesheet& s);
  virtual void write(CssWriter &writer);
};

#include "RulesetStatement.h"
#include "Declaration.h"

#endif
