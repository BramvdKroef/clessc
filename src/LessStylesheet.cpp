#include "LessStylesheet.h"
#include "LessMediaQuery.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

LessStylesheet::LessStylesheet() {
}

LessStylesheet::~LessStylesheet() {
  while (!parameterRulesets.empty()) {
    delete parameterRulesets.back();
    parameterRulesets.pop_back();
  }
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
  
void LessStylesheet::addParameterRuleset(ParameterRuleset* rule) {
  DLOG(INFO) << "Adding parameter ruleset: " <<
    *rule->getSelector()->toString();

  rule->setStylesheet(this);
  parameterRulesets.push_back(rule);
}

LessRuleset* LessStylesheet::getLessRuleset(Selector* selector) {
  vector<LessRuleset*>::iterator it;

  for (it = lessrulesets.begin(); it != lessrulesets.end(); it++) {
    //DLOG(INFO) << *(*it)->getSelector()->toString();
    if ((*it)->getSelector()->equals(selector)) 
      return *it;
  }
  return NULL;
}

list<ParameterRuleset*> LessStylesheet::getParameterRulesets(ParameterMixin* mixin) {
  vector<ParameterRuleset*>::iterator pri;
  list<ParameterRuleset*> prulesetList;
  
  for (pri = parameterRulesets.begin(); pri != parameterRulesets.end();
       pri++) {
    
    if ((*pri)->getSelector()->equals(mixin->name) &&
        (*pri)->matchArguments(mixin->arguments)) {
      prulesetList.push_back(*pri);
    }
  }
  return prulesetList;
}


ValueProcessor* LessStylesheet::getValueProcessor() {
  return &valueProcessor;
}

void LessStylesheet::putVariable(string key, TokenList* value) {
  DLOG(INFO) << "Variable: " << key << ": " << *value->toString();
  getValueProcessor()->putVariable(key, value);
}
