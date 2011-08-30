#include "Stylesheet.h"

virtual Declaration::~Declaration() {
  string* token = NULL;

  delete property;
    
  while ((token = value->back())) {
    value->pop_back();
    delete token;
  }
  delete value;
}

virtual Ruleset::~Ruleset() {
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

virtual AtRule::~AtRule() {
  string* token = NULL;

  delete keyword;
    
  while ((token = rule->back())) {
    rule->pop_back();
    delete token;
  }
  delete rule;
}

virtual Stylesheet::~Stylesheet() {
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

void addAtRule(AtRule* atrule) {
  atrules->pop_back(atrule);
}
void addRuleset(RuleSet* ruleset) {
  rulesets->pop_back(ruleset);
}
  
