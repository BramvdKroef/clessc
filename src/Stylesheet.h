#ifndef __Stylesheet_h__
#define __Stylesheet_h__

#include <string>
#include <vector>

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
public:
  string* keyword;
  vector<string*>* rule;
  
  virtual ~AtRule();
};

class Stylesheet {
public:
  Stylesheet() {}
  virtual ~Stylesheet();
  vector<AtRule*> atrules;
  vector<Ruleset*> rulesets;
};

#endif
