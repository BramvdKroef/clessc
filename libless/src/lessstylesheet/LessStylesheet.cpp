#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/LessMediaQuery.h"

LessStylesheet::LessStylesheet() {
}

LessStylesheet::~LessStylesheet() {
}

LessRuleset* LessStylesheet::createLessRuleset() {
  LessRuleset* r = new LessRuleset();

  addRuleset(*r);
  lessrulesets.push_back(r);
  r->setLessStylesheet(*this);
  return r;
}

Mixin* LessStylesheet::createMixin() {
  Mixin* m = new Mixin();

  addStatement(*m);
  m->setLessStylesheet(*this);
  return m;
}

LessAtRule* LessStylesheet::createLessAtRule(const Token& keyword) {
  LessAtRule* atrule = new LessAtRule(keyword);
  addAtRule(*atrule);
  atrule->setLessStylesheet(*this);
  return atrule;
}

LessMediaQuery* LessStylesheet::createLessMediaQuery() {
  LessMediaQuery* q = new LessMediaQuery();

  addStatement(*q);
  q->setLessStylesheet(*this);
  return q;
}

void LessStylesheet::deleteLessRuleset(LessRuleset& ruleset) {
  lessrulesets.remove(&ruleset);
  deleteStatement(ruleset);
}

void LessStylesheet::deleteMixin(Mixin& mixin) {
  deleteStatement(mixin);
}

void LessStylesheet::getFunctions(std::list<const Function*>& functionList,
                                  const Mixin& mixin,
                                  const ProcessingContext &context) const {
  std::list<LessRuleset*>::const_iterator i;
  const std::list<Closure*>* closures;
  std::list<Closure*>::const_iterator c_it;

  for (i = lessrulesets.begin(); i != lessrulesets.end(); i++) {
    (*i)->getFunctions(functionList, mixin, mixin.name.begin(), context);
  }
  
  closures = context.getBaseClosures();
  for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
    (*c_it)->getFunctions(functionList, mixin, mixin.name.begin(), context);
  }
}

void LessStylesheet::putVariable(const std::string& key,
                                 const TokenList& value) {
  variables[key] = value;
}
const TokenList* LessStylesheet::getVariable(const std::string& key) const {
  return variables.getVariable(key);
}
const TokenList* LessStylesheet::getVariable(const std::string& key,
                                             const ProcessingContext &context) const {
  const TokenList* t;

  if ((t = getVariable(key)) != NULL)
    return t;

  return context.getBaseVariable(key);
}

void LessStylesheet::process(Stylesheet& s, void* context) const {
  std::list<Extension>* extensions;

  std::list<Ruleset*>::const_iterator r_it;
  std::list<Extension>::iterator e_it;
  std::list<Closure*> closureScope;

  ((ProcessingContext*)context)->setLessStylesheet(*this);
  Stylesheet::process(s, context);

  // post processing
  extensions = &((ProcessingContext*)context)->getExtensions();

  for (r_it = s.getRulesets().begin(); r_it != s.getRulesets().end(); r_it++) {
    for (e_it = extensions->begin(); e_it != extensions->end(); e_it++) {
      (*e_it).updateSelector((*r_it)->getSelector());
    }
  }
}

