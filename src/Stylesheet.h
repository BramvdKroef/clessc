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
#include <list>

class CssWritable {
public:
  virtual void write(CssWriter &css) = 0; 
};
  
class Ruleset; 

class RulesetStatement :  public CssWritable {
protected:
  Ruleset* ruleset;
public:
  virtual ~RulesetStatement() {};

  virtual void setRuleset(Ruleset *r);
  Ruleset* getRuleset();
  virtual void process(Ruleset &r) = 0;
};

class Declaration: public RulesetStatement {
private:
  std::string property;
  TokenList value;
  
public:
  Declaration();
  Declaration(const std::string &property);
  virtual ~Declaration();
  void setProperty(const std::string &property);
  void setValue(const TokenList &value);
  
  std::string getProperty();
  TokenList& getValue();

  virtual void process(Ruleset &r);
  virtual void write(CssWriter &writer);
};

class Stylesheet;

class StylesheetStatement : public CssWritable  {
protected:
  Stylesheet* stylesheet;
  
public:
  virtual ~StylesheetStatement() {};
  virtual void setStylesheet(Stylesheet* s);
  Stylesheet* getStylesheet();
  
  virtual void process(Stylesheet &s) = 0;
};

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
  Declaration* createDeclaration(const std::string &property);

  void deleteDeclaration(Declaration &declaration);
  
  void addDeclarations (std::list<Declaration> &declarations);

  Selector& getSelector();
  const Selector& getSelector() const;
  std::list<RulesetStatement*>& getStatements();
  std::list<Declaration*>& getDeclarations();

  void clearStatements();
  
  virtual void insert(Ruleset &target);
  virtual void process(Stylesheet& s);
  virtual void write(CssWriter &writer);
};

class AtRule: public StylesheetStatement {
private:
  std::string keyword;
  TokenList rule;

public:
  AtRule(const std::string& keyword);
  virtual ~AtRule();
  void setKeyword (const std::string &keyword);
  void setRule(const TokenList &rule);

  std::string& getKeyword();
  TokenList& getRule();

  virtual void process(Stylesheet &s);
  virtual void write(CssWriter &writer);
};

class MediaQuery;

class Stylesheet: public CssWritable {
private:
  std::list<AtRule*> atrules;
  std::list<Ruleset*> rulesets;
  std::list<StylesheetStatement*> statements;

protected:
  virtual void addStatement(StylesheetStatement &statement);
  virtual void addRuleset(Ruleset &ruleset);
  virtual void addAtRule(AtRule &rule);
  void deleteStatement(StylesheetStatement &statement);
  
public:
  Stylesheet() {}
  virtual ~Stylesheet();
  
  Ruleset* createRuleset();
  Ruleset* createRuleset(const Selector &selector);

  AtRule* createAtRule(const std::string &keyword);
  virtual MediaQuery* createMediaQuery();

  void deleteRuleset(Ruleset &ruleset);
  void deleteAtRule(AtRule &atrule);
  void deleteMediaQuery(MediaQuery &query);
  
  std::list<AtRule*>& getAtRules();
  std::list<Ruleset*>& getRulesets();
  std::list<StylesheetStatement*>& getStatements();
  
  virtual Ruleset* getRuleset(const Selector& selector);

  virtual void process(Stylesheet &s);
  virtual void write(CssWriter &writer);
};

class MediaQuery: public Stylesheet, public StylesheetStatement {
private:
  Selector selector;

public:
  Selector& getSelector();
  void setSelector(const Selector& s);

  virtual MediaQuery* createMediaQuery();
  
  virtual void process(Stylesheet& s);
  virtual void write(CssWriter &writer);
};

#endif
