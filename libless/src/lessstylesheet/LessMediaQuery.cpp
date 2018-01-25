#include "less/lessstylesheet/LessMediaQuery.h"

LessMediaQuery::LessMediaQuery() {
  parent = NULL;
}
LessMediaQuery::~LessMediaQuery() {
}

Selector *LessMediaQuery::getSelector() {
  return &selector;
}
const Selector *LessMediaQuery::getSelector() const {
  return &selector;
}
void LessMediaQuery::setSelector(const Selector &s) {
  selector = s;
}

void LessMediaQuery::setLessStylesheet(LessStylesheet &parent) {
  this->parent = &parent;
}
LessStylesheet *LessMediaQuery::getLessStylesheet() const {
  return parent;
}

void LessMediaQuery::getFunctions(std::list<const Function *> &functionList,
                                  const Mixin &mixin) const {
  LessStylesheet::getFunctions(functionList, mixin);
  getLessStylesheet()->getFunctions(functionList, mixin);
}

const TokenList *LessMediaQuery::getVariable(const std::string &key) const {
  const TokenList *t = LessStylesheet::getVariable(key);
  if (t == NULL)
    t = getLessStylesheet()->getVariable(key);
  return t;
}

void LessMediaQuery::process(Stylesheet &s, void* context) const {
  MediaQuery *query = s.createMediaQuery();

  query->setSelector(*getSelector());
  ((ProcessingContext*)context)->processValue(query->getSelector());
  
  LessStylesheet::process(*query, ((ProcessingContext*)context));
  ((ProcessingContext*)context)->setLessStylesheet(*parent);
}

void LessMediaQuery::write(CssWriter &writer) const {
  LessStylesheet::write(writer);
}
