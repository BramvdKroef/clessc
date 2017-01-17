#include "LessMediaQuery.h"

LessMediaQuery::LessMediaQuery() {
  parent = NULL;
}
LessMediaQuery::~LessMediaQuery() {
}

Selector* LessMediaQuery::getSelector() {
  return &selector;
}
void LessMediaQuery::setSelector(const Selector &s) {
  selector = s;
}

void LessMediaQuery::setLessStylesheet(LessStylesheet &parent) {
  this->parent = &parent;
}
LessStylesheet* LessMediaQuery::getLessStylesheet() {
  return parent;
}

void LessMediaQuery::getLessRulesets(std::list<LessRuleset*> &rulesetList,
                                     const Mixin &mixin) {
  LessStylesheet::getLessRulesets(rulesetList, mixin);
  getLessStylesheet()->getLessRulesets(rulesetList, mixin);
}

ProcessingContext* LessMediaQuery::getContext() {
  return getLessStylesheet()->getContext();
}
void LessMediaQuery::process(Stylesheet &s) {
  MediaQuery* query = s.createMediaQuery();

  query->setSelector(*getSelector());
  getContext()->processValue(query->getSelector());

  LessStylesheet::process(*query, *parent->getContext());
}


void LessMediaQuery::write(CssWriter &writer) {
  LessStylesheet::write(writer);
}
