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
  virtual ~Declaration();
  void setProperty(string* property);
  void setValue(vector<string*>* value);
};
  
class Ruleset {
private:
  vector<string*>* selector;
  vector<Declaration*> declarations;
  
public:
  void setSelector (vector<string*>* selector);
  void addDeclaration (Declaration* declaration);
  virtual ~Ruleset();
};

class AtRule {
private:
  string* keyword = NULL;
  vector<string*>* rule = NULL;

public:
  AtRule(string* keyword);
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
