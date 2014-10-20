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
#ifdef WITH_LIBGLOG
  VLOG(3) << "Adding LessRuleset";
#endif
  
  Stylesheet::addStatement(ruleset);
  lessrulesets.push_back(ruleset);
  ruleset->setStylesheet(this);
}
void LessStylesheet::addStatement(ParameterMixin* mixin) {
#ifdef WITH_LIBGLOG
  VLOG(3) << "Adding Parameter mixin";
#endif
  
  Stylesheet::addStatement(mixin);
  mixin->setStylesheet(this);
}

void LessStylesheet::addStatement(LessMediaQuery* query) {
#ifdef WITH_LIBGLOG
  VLOG(3) << "Adding Media Query";
#endif
  
  Stylesheet::addStatement(query);
  query->setStylesheet(this);
}
  
void LessStylesheet::getLessRulesets(list<LessRuleset*>* rulesetList,
                                     ParameterMixin* mixin) {
  vector<LessRuleset*>::iterator i;
  
  for (i = lessrulesets.begin(); i != lessrulesets.end();
       i++) {

    (*i)->getLessRulesets(rulesetList, mixin, 0);
  }
}

void LessStylesheet::getExtensions(map<string,TokenList*>* extensions) {
  vector<LessRuleset*>::iterator i;
  
  for (i = lessrulesets.begin(); i != lessrulesets.end();
       i++) {

    (*i)->getExtensions(extensions, NULL);
  }
}

ValueProcessor* LessStylesheet::getValueProcessor() {
  return &valueProcessor;
}

void LessStylesheet::putVariable(string key, TokenList* value) {
  variables.insert(pair<string, TokenList*>(key, value));  
}

void LessStylesheet::processVariables() {
  map<string, TokenList*>::iterator it;

  for (it = variables.begin(); it != variables.end(); ++it) {
#ifdef WITH_LIBGLOG
    VLOG(3) << "Variable: " << it->first << ": " << *it->second->toString();
#endif
    getValueProcessor()->putVariable(it->first, it->second);
  }
}

void LessStylesheet::process(Stylesheet* s) {
  processVariables();
  Stylesheet::process(s);
}
