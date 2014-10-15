#include "LessMediaQuery.h"

LessMediaQuery::LessMediaQuery() {
  parent = NULL;
  selector = NULL;
}
LessMediaQuery::~LessMediaQuery() {
  if (selector != NULL)
    delete selector;
}

Selector* LessMediaQuery::getSelector() {
  return selector;
}
void LessMediaQuery::setSelector(Selector* s) {
  selector = s;
}

void LessMediaQuery::setStylesheet(LessStylesheet* parent) {
  this->parent = parent;
}
LessStylesheet* LessMediaQuery::getLessStylesheet() {
  return parent;
}

list<LessRuleset*> LessMediaQuery::getLessRulesets(ParameterMixin* mixin) {
  list<LessRuleset*> rulesetList1 = LessStylesheet::getLessRulesets(mixin);
  list<LessRuleset*> rulesetList2 = getLessStylesheet()->getLessRulesets(mixin);

  rulesetList1.insert(rulesetList1.end(),
                      rulesetList2.begin(), rulesetList2.end());
  return rulesetList1;
}

ValueProcessor* LessMediaQuery::getValueProcessor() {
  return getLessStylesheet()->getValueProcessor();
}
void LessMediaQuery::process(Stylesheet* s) {
  MediaQuery* query = new MediaQuery();

  query->setSelector(getSelector()->clone());
  s->addStatement(query);

  Stylesheet::process(query);
}


void LessMediaQuery::write(CssWriter* writer) {
  LessStylesheet::write(writer);
}
