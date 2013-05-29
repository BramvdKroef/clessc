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

#ifndef __Stylesheet_h__
#define __Stylesheet_h__

#include "Token.h"
#include "TokenList.h"
#include "Selector.h"
#include <string>
#include <vector>

using namespace std;

class Declaration {
private:
  string* property;
  TokenList* value;
  
public:
  Declaration(string* property);
  virtual ~Declaration();
  void setProperty(string* property);
  void setValue(TokenList* value);
  
  string* getProperty();
  TokenList* getValue();
  Declaration* clone();
};
  
class Ruleset {
private:
  Selector* selector;
  vector<Declaration*> declarations;
  
public:
  Ruleset ();
  Ruleset(Selector* selector);
  virtual ~Ruleset();
  void setSelector (Selector* selector);
  void addDeclaration (Declaration* declaration);
  void addDeclarations (vector<Declaration*>* declarations);
    
  Selector* getSelector();
  vector<Declaration*>* getDeclarations();
  vector<Declaration*>* cloneDeclarations();
  Ruleset* clone();
};

class AtRule {
private:
  string* keyword;
  TokenList* rule;

public:
  AtRule(string* keyword);
  virtual ~AtRule();
  void setKeyword (string* keyword);
  void setRule(TokenList* rule);

  string* getKeyword();
  TokenList* getRule();
};

class Stylesheet {
private:
  vector<AtRule*> atrules;
  vector<Ruleset*> rulesets;
  
public:
  Stylesheet() {}
  virtual ~Stylesheet();
  
  void addAtRule(AtRule* atrule);
  void addRuleset(Ruleset* ruleset);

  vector<AtRule*>* getAtRules();
  vector<Ruleset*>* getRulesets();

  Ruleset* getRuleset(Selector* selector);
};

#endif
