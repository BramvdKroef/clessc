#include "LessStylesheet.h"
#include "LessMediaQuery.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

LessStylesheet::LessStylesheet() {
}

LessStylesheet::~LessStylesheet() {
}

void LessStylesheet::addStatement(AtRule* atrule) {
  Stylesheet::addStatement(atrule);
  atrule->setStylesheet(this);
}

void LessStylesheet::addStatement(LessRuleset* ruleset) {
  DLOG(INFO) << "Adding LessRuleset";
  Stylesheet::addStatement(ruleset);
  lessrulesets.push_back(ruleset);
  ruleset->setStylesheet(this);
}
void LessStylesheet::addStatement(ParameterMixin* mixin) {
  DLOG(INFO) << "Adding Parameter mixin";
  Stylesheet::addStatement(mixin);
  mixin->setStylesheet(this);
}

void LessStylesheet::addStatement(LessMediaQuery* query) {
  DLOG(INFO) << "Adding Media Query";
  Stylesheet::addStatement(query);
  query->setStylesheet(this);
}
  
list<LessRuleset*> LessStylesheet::getLessRulesets(ParameterMixin* mixin) {
  vector<LessRuleset*>::iterator i;
  list<LessRuleset*> rulesetList;
  
  for (i = lessrulesets.begin(); i != lessrulesets.end();
       i++) {

    (*i)->getLessRulesets(&rulesetList, mixin, 0);
  }
  return rulesetList;
}

void LessStylesheet::getExtensions(map<string,TokenList*>* extensions) {
  vector<LessRuleset*>::iterator i;
  
  for (i = lessrulesets.begin(); i != lessrulesets.end();
       i++) {

    (*i)->getExtensions(extensions);
  }
}

ValueProcessor* LessStylesheet::getValueProcessor() {
  return &valueProcessor;
}

void LessStylesheet::putVariable(string key, TokenList* value) {
  DLOG(INFO) << "Variable: " << key << ": " << *value->toString();
  getValueProcessor()->putVariable(key, value);
}
