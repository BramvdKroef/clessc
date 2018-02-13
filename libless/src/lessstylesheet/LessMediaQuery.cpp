#include "less/lessstylesheet/LessMediaQuery.h"
#include "less/stylesheet/MediaQuery.h"

LessMediaQuery::LessMediaQuery(const TokenList &selector,
                               const LessStylesheet &parent) :
  selector(selector), parent(&parent) {

}
LessMediaQuery::~LessMediaQuery() {
}

TokenList &LessMediaQuery::getSelector() {
  return selector;
}
const TokenList &LessMediaQuery::getSelector() const {
  return selector;
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
  MediaQuery *query;
  
  query = s.createMediaQuery(getSelector());
  ((ProcessingContext*)context)->processValue(query->getSelector());
    
  LessStylesheet::process(*query, ((ProcessingContext*)context));
  ((ProcessingContext*)context)->setLessStylesheet(*parent);
}

void LessMediaQuery::write(CssWriter &writer) const {
  LessStylesheet::write(writer);
}
