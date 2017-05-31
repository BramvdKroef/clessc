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
LessStylesheet* LessMediaQuery::getLessStylesheet() const {
  return parent;
}

void LessMediaQuery::getFunctions(std::list<const Function*> &functionList,
                                     const Mixin &mixin) const {
  LessStylesheet::getFunctions(functionList, mixin);
  getLessStylesheet()->getFunctions(functionList, mixin);
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
