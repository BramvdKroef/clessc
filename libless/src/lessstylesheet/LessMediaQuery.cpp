#include "less/lessstylesheet/LessMediaQuery.h"

LessMediaQuery::LessMediaQuery(const Selector &selector,
                               const LessStylesheet &parent) :
  parent(&parent), selector(selector) {

}
LessMediaQuery::~LessMediaQuery() {
}

const LessStylesheet &LessMediaQuery::getLessStylesheet() const {
  return *parent;
}

void LessMediaQuery::getFunctions(std::list<const Function *> &functionList,
                                  const Mixin &mixin,
                                  const ProcessingContext &context) const {
  LessStylesheet::getFunctions(functionList, mixin, context);
  getLessStylesheet().getFunctions(functionList, mixin, context);
}

const TokenList *LessMediaQuery::getVariable(const std::string &key,
                                             const ProcessingContext &context) const {
  const TokenList *t = LessStylesheet::getVariable(key, context);
  if (t == NULL)
    t = getLessStylesheet().getVariable(key, context);
  return t;
}

void LessMediaQuery::process(Stylesheet &s, void* context) const {
  Selector selector = *getSelector();
  MediaQuery *query;

  ((ProcessingContext*)context)->processValue(selector);

  query = s.createMediaQuery(selector);
    
  LessStylesheet::process(*query, ((ProcessingContext*)context));
  ((ProcessingContext*)context)->setLessStylesheet(*parent);
}

void LessMediaQuery::write(CssWriter &writer) const {
  LessStylesheet::write(writer);
}
