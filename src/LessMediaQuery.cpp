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

Ruleset* LessMediaQuery::getRuleset(Selector* selector) {
  Ruleset* r = Stylesheet::getRuleset(selector);
  if (r == NULL)
    r = getLessStylesheet()->getRuleset(selector);
  return r;
}

LessRuleset* LessMediaQuery::getLessRuleset(Selector* selector) {
  LessRuleset* r = LessStylesheet::getLessRuleset(selector);
  if (r == NULL)
    r = getLessStylesheet()->getLessRuleset(selector);
  return r;
}

list<ParameterRuleset*> LessMediaQuery::getParameterRulesets(ParameterMixin* mixin) {
  list<ParameterRuleset*> prulesetList1, prulesetList2;
  prulesetList1 = LessStylesheet::getParameterRulesets(mixin);
  prulesetList2 = getLessStylesheet()->getParameterRulesets(mixin);
  prulesetList1.insert(prulesetList1.end(),
                       prulesetList2.begin(),prulesetList2.end());
  return prulesetList1;
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
