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
#include "CssWriter.h"
#include <string>
#include <vector>


using namespace std;

class CssWritable {
public:
  virtual void write(CssWriter* css) = 0; 
};
  
class Ruleset; 

class RulesetStatement :  public CssWritable {
protected:
  Ruleset* ruleset;
public:
  virtual ~RulesetStatement() {};

  virtual void setRuleset(Ruleset* r);
  Ruleset* getRuleset();

  virtual RulesetStatement* clone() = 0;
  virtual void process(Ruleset* r) = 0;
};

class Declaration: public RulesetStatement {
private:
  string* property;
  TokenList* value;
  
public:
  Declaration();
  Declaration(string* property);
  virtual ~Declaration();
  void setProperty(string* property);
  void setValue(TokenList* value);
  
  string* getProperty();
  TokenList* getValue();
  virtual Declaration* clone();

  virtual void process(Ruleset* r);
  virtual void write(CssWriter* writer);
};

class Stylesheet;

class StylesheetStatement : public CssWritable  {
protected:
  Stylesheet* stylesheet;
  
public:
  virtual ~StylesheetStatement() {};
  virtual void setStylesheet(Stylesheet* s);
  Stylesheet* getStylesheet();

  virtual void process(Stylesheet* s) = 0;
};

class Ruleset: public StylesheetStatement {
protected:
  Selector* selector;
  vector<RulesetStatement*> statements;
  vector<Declaration*> declarations;
  
public:
  Ruleset ();
  Ruleset(Selector* selector);
  virtual ~Ruleset();
  void setSelector (Selector* selector);

  virtual void addStatement (RulesetStatement* statement);
  virtual void addStatement(Declaration* declaration);
    
  void addStatements (vector<RulesetStatement*>* statements);
  void addDeclarations (vector<Declaration*>* declarations);
    
  Selector* getSelector();
  vector<RulesetStatement*>* getStatements();
  vector<Declaration*>* getDeclarations();

  void clearStatements();
  
  vector<Declaration*>* cloneDeclarations();
  Ruleset* clone();
  void swap(Ruleset* ruleset);

  virtual void insert(Ruleset* target);
  virtual void process(Stylesheet* s);
  virtual void write(CssWriter* writer);
};

class AtRule: public StylesheetStatement {
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

  virtual void process(Stylesheet* s);
  virtual void write(CssWriter* writer);
};


class Stylesheet: public CssWritable {
private:
  vector<AtRule*> atrules;
  vector<Ruleset*> rulesets;
  vector<StylesheetStatement*> statements;
public:
  Stylesheet() {}
  virtual ~Stylesheet();
  
  virtual void addStatement(StylesheetStatement* statement);
  virtual void addStatement(Ruleset* ruleset);
  virtual void addStatement(AtRule* rule);

  vector<AtRule*>* getAtRules();
  vector<Ruleset*>* getRulesets();
  vector<StylesheetStatement*>* getStatements();
  
  Ruleset* getRuleset(Selector* selector);

  virtual void process(Stylesheet* s);
  virtual void write(CssWriter* writer);
};

class MediaQuery: public Stylesheet, public StylesheetStatement {
private:
  Selector* selector;

public:
  Selector* getSelector();
  void setSelector(Selector* s);
  virtual void process(Stylesheet* s);
  virtual void write(CssWriter* writer);
}

#endif
