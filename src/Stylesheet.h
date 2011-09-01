#ifndef __Stylesheet_h__
#define __Stylesheet_h__

#include "Token.h"
#include "TokenList.h"
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
  TokenList* selector;
  vector<Declaration*> declarations;
  
public:
  virtual ~Ruleset();
  void setSelector (TokenList* selector);
  void addDeclaration (Declaration* declaration);

  TokenList* getSelector();
  vector<Declaration*>* getDeclarations();
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

  Ruleset* getRuleset(TokenList* selector);
};

#endif
