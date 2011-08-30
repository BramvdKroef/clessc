#ifndef __Stylesheet_h__
#define __Stylesheet_h__

#include <string>
#include <vector>
using namespace std;

class Declaration {
public:
  string* property;
  vector<string*>* value;

  virtual ~Declaration();
};
  
class Ruleset {
public:
  vector<string*>* selector;
  vector<Declaration*> declarations;

  virtual ~Ruleset();
};

class AtRule {
private:
  string* keyword;
  vector<string*>* rule;

public:
  virtual ~AtRule();
  void setKeyword (string* keyword);
  void setRule(vector<string*>* rule);
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
};

#endif
