#ifndef __Stylesheet_h__
#define __Stylesheet_h__

#include <string>
#include <vector>

class Declaration {
public:
  string* property;
  vector<string*>* value;

  virtual ~Declaration() {
    string* token = NULL;

    delete property;
    
    while ((token = value->back())) {
      value->pop_back();
      delete token;
    }
    delete value;
  }
};
  
class Ruleset {
public:
  vector<string*>* selector;
  vector<Declaration*> declarations;

  virtual ~Ruleset() {
    string* token = NULL;
    Declaration* declaration = NULL;

    if (selector != NULL) {
      while ((token = selector->back())) {
        selector->pop_back();
        delete token;
      }
      delete selector;
    }
    while ((declaration = declarations.back())) {
      declarations.pop_back();
      delete declaration;
    }
  }
};

class AtRule {
public:
  string* keyword;
  vector<string*>* rule;
  
  virtual ~AtRule() {
    string* token = NULL;

    delete keyword;
    
    while ((token = rule->back())) {
      rule->pop_back();
      delete token;
    }
    delete rule;
  }
};

class Stylesheet {
public:
  Stylesheet() {}
  virtual ~Stylesheet() {
    AtRule* rule = NULL;
    Ruleset* ruleset = NULL;
    
    while ((rule = atrules.back())) {
      atrules.pop_back();
      delete rule;
    }
    while ((ruleset = rulesets.back())) {
      rulesets.pop_back();
      delete ruleset;
    }
  }
  vector<AtRule*> atrules;
  vector<Ruleset*> rulesets;
};

#endif
