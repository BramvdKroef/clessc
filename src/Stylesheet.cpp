#include "Stylesheet.h"

Declaration::~Declaration() {
  string* token = NULL;

  delete property;
    
  while ((token = value->back())) {
    value->pop_back();
    delete token;
  }
  delete value;
}

void Declaration::setProperty(string* property) {
  this->property = property;
}
void Declaration::setValue(vector<string*>* value) {
  this->value = value;
}


Ruleset::~Ruleset() {
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

void Ruleset::setSelector (vector<string*>* selector) {
  this->selector = selector;
}
void Ruleset::addDeclaration (Declaration* declaration) {
  declarations.push_back(declaration);
}


AtRule::~AtRule() {
  string* token = NULL;

  delete keyword;
    
  while ((token = rule->back())) {
    rule->pop_back();
    delete token;
  }
  delete rule;
}

void AtRule::setKeyword (string* keyword) {
  this->keyword = keyword;
}
void AtRule::setRule(vector<string*>* rule) {
  this->rule = rule;
}


Stylesheet::~Stylesheet() {
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

void Stylesheet::addRuleset(Ruleset* ruleset) {
  rulesets.push_back(ruleset);
}

void Stylesheet::addAtRule(AtRule* atrule) {
  atrules.push_back(atrule);
}

