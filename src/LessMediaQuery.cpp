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

void LessMediaQuery::getLessRulesets(list<LessRuleset*>* rulesetList,
                                     Mixin* mixin) {
  LessStylesheet::getLessRulesets(rulesetList, mixin);
  getLessStylesheet()->getLessRulesets(rulesetList, mixin);
}

ValueProcessor* LessMediaQuery::getValueProcessor() {
  return getLessStylesheet()->getValueProcessor();
}
void LessMediaQuery::process(Stylesheet* s) {
  MediaQuery* query = new MediaQuery();

  query->setSelector(getSelector()->clone());
  getValueProcessor()->processValue(query->getSelector());
  s->addStatement(query);

  LessStylesheet::process(query);
}


void LessMediaQuery::write(CssWriter* writer) {
  LessStylesheet::write(writer);
}
