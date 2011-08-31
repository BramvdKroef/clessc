#ifndef __Stylesheet_h__
#define __Stylesheet_h__

#include "Token.h"
#include <string>
#include <vector>
using namespace std;

class Declaration {
private:
  string* property;
  vector<Token*>* value;
  
public:
  Declaration(string* property);
  virtual ~Declaration();
  void setProperty(string* property);
  void setValue(vector<Token*>* value);
  
  string* getProperty();
  vector<Token*>* getValue();
};
  
class Ruleset {
private:
  vector<Token*>* selector;
  vector<Declaration*> declarations;
  
public:
  virtual ~Ruleset();
  void setSelector (vector<Token*>* selector);
  void addDeclaration (Declaration* declaration);

  vector<Token*>* getSelector();
  vector<Declaration*>* getDeclarations();
};

class AtRule {
private:
  string* keyword;
  vector<Token*>* rule;

public:
  AtRule(string* keyword);
  virtual ~AtRule();
  void setKeyword (string* keyword);
  void setRule(vector<Token*>* rule);

  string* getKeyword();
  vector<Token*>* getRule();
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
};

#endif
