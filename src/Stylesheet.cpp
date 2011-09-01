#include "Stylesheet.h"

Declaration::Declaration(string* property) {
  this->property = property;
  value = NULL;
}

Declaration::~Declaration() {
  if (property != NULL)
    delete property;

  if (value != NULL) {
    while (!value->empty()) 
      delete value->pop();
    delete value;
  }
}

void Declaration::setProperty(string* property) {
  this->property = property;
}
void Declaration::setValue(TokenList* value) {
  this->value = value;
}
string* Declaration::getProperty() {
  return property;
}
TokenList* Declaration::getValue() {
  return value;
}
Declaration* Declaration::clone() {
  Declaration* clone =
    new Declaration(new string(*this->getProperty()));
  
  clone->setValue(value->clone());
  return clone;
}

Ruleset::~Ruleset() {
  if (selector != NULL) {
    delete selector;
  }
  
  while (!declarations.empty()) {
    delete declarations.back();
    declarations.pop_back();
  }
}

void Ruleset::setSelector (TokenList* selector) {
  this->selector = selector;
}
void Ruleset::addDeclaration (Declaration* declaration) {
  declarations.push_back(declaration);
}
TokenList* Ruleset::getSelector() {
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

  if (rule != NULL) 
    delete rule;
}

void AtRule::setKeyword (string* keyword) {
  this->keyword = keyword;
}
void AtRule::setRule(TokenList* rule) {
  this->rule = rule;
}
string* AtRule::getKeyword() {
  return keyword;
}
TokenList* AtRule::getRule() {
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
Ruleset* Stylesheet::getRuleset(TokenList* selector) {
  vector<Ruleset*>::iterator it;
  for (it = rulesets.begin(); it < rulesets.end(); it++) {
    if ((*it)->getSelector()->equals(selector)) 
      return *it;
  }
  return NULL;
}
