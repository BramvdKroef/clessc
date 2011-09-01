#include "Stylesheet.h"

Declaration::Declaration(string* property) {
  this->property = property;
  value = NULL;
}

Declaration::~Declaration() {
  if (property != NULL)
    delete property;

  if (value != NULL) {
    while (!value->empty()) {
      delete value->back();
      value->pop_back();
    }
    delete value;
  }
}

void Declaration::setProperty(string* property) {
  this->property = property;
}
void Declaration::setValue(vector<Token*>* value) {
  this->value = value;
}
string* Declaration::getProperty() {
  return property;
}
vector<Token*>* Declaration::getValue() {
  return value;
}
Declaration* Declaration::clone() {
  Declaration* clone = new Declaration(new
                                       string(this->getProperty()));
  vector<Token*>* newvalue = new vector<Token*>();
  vector<Token*>::iterator it;
  for (it = value->begin(); it < value->end(); it++) {
    newvalue->push_back((*it)->clone());
  }
  clone->setValue(newvalue);
  return clone;
}

Ruleset::~Ruleset() {
  if (selector != NULL) {
    while (!selector->empty()) {
      delete selector->back();
      selector->pop_back();
    }
    delete selector;
  }
  
  while (!declarations.empty()) {
    delete declarations.back();
    declarations.pop_back();
  }
}

void Ruleset::setSelector (vector<Token*>* selector) {
  this->selector = selector;
}
void Ruleset::addDeclaration (Declaration* declaration) {
  declarations.push_back(declaration);
}
vector<Token*>* Ruleset::getSelector() {
  return selector;
}
vector<Declaration*>* Ruleset::getDeclarations() {
  return &declarations;
}

AtRule::AtRule (string* keyword) {
  this->keyword = keyword;
  rule = NULL;
}

AtRule::~AtRule() {
  if (keyword != NULL)
    delete keyword;

  if (rule != NULL) {
    while (!rule->empty()) {
      delete rule->back();
      rule->pop_back();
    }
    delete rule;
  }
}

void AtRule::setKeyword (string* keyword) {
  this->keyword = keyword;
}
void AtRule::setRule(vector<Token*>* rule) {
  this->rule = rule;
}
string* AtRule::getKeyword() {
  return keyword;
}
vector<Token*>* AtRule::getRule() {
  return rule;
}


Stylesheet::~Stylesheet() {
  while (!atrules.empty()) {
    delete atrules.back();
    atrules.pop_back();
  }
  while (!rulesets.empty()) {
    delete rulesets.back();
    rulesets.pop_back();
  }
}

void Stylesheet::addRuleset(Ruleset* ruleset) {
  rulesets.push_back(ruleset);
}

void Stylesheet::addAtRule(AtRule* atrule) {
  atrules.push_back(atrule);
}
vector<AtRule*>* Stylesheet::getAtRules() {
  return &atrules;
}
vector<Ruleset*>* Stylesheet::getRulesets() {
  return &rulesets;
}
Ruleset* Stylesheet::getRuleset(vector<Token*> selector) {
  return NULL;
}
