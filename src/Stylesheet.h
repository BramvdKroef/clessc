#ifndef __Stylesheet_h__
#define __Stylesheet_h__

#include <string>
#include <vector>
using namespace std;

class Declaration {
private:
  string* property;
  vector<string*>* value;
  
public:
  Declaration(string* property);
  virtual ~Declaration();
  void setProperty(string* property);
  void setValue(vector<string*>* value);
  
  string* getProperty();
  vector<string*>* getValue();
};
  
class Ruleset {
private:
  vector<string*>* selector;
  vector<Declaration*> declarations;
  
public:
  virtual ~Ruleset();
  void setSelector (vector<string*>* selector);
  void addDeclaration (Declaration* declaration);

  vector<string*>* getSelector();
  vector<Declaration*>* getDeclarations();
};

class AtRule {
private:
  string* keyword;
  vector<string*>* rule;

public:
  AtRule(string* keyword);
  virtual ~AtRule();
  void setKeyword (string* keyword);
  void setRule(vector<string*>* rule);

  string* getKeyword();
  vector<string*>* getRule();
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
