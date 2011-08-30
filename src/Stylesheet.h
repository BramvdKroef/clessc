#ifndef __Stylesheet_h__
#define __Stylesheet_h__

class Declaration {
  string* property;
  vector<string*>* value;
}
  
class Ruleset {
  vector<string*>* selector;
  vector<Declaration*> declarations;
}

class AtRule {
  string* keyword;
  vector<string*>* rule;
}

class Stylesheet {
private:
  vector<AtRule*> atrules;
  vector<Ruleset*> rulesets;
public:
  Stylesheet();
  virtual ~Stylesheet();
}

#endif
