#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/LessMediaQuery.h"

LessStylesheet::LessStylesheet() {
}

LessStylesheet::~LessStylesheet() {
}

LessRuleset* LessStylesheet::createLessRuleset(const LessSelector &selector) {
  LessRuleset* r = new LessRuleset(selector, *this);

  addRuleset(*r);
  lessrulesets.push_back(r);
  return r;
}

Mixin* LessStylesheet::createMixin(const Selector &selector) {
  Mixin* m = new Mixin(selector, *this);

  addStatement(*m);
  return m;
}

LessAtRule* LessStylesheet::createLessAtRule(const Token& keyword) {
  LessAtRule* atrule = new LessAtRule(keyword);
  addAtRule(*atrule);
  return atrule;
}

LessMediaQuery* LessStylesheet::createLessMediaQuery(const Selector &selector) {
  LessMediaQuery* q = new LessMediaQuery(selector, *this);

  addStatement(*q);
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
  Selector* selector;

  ((ProcessingContext*)context)->setLessStylesheet(*this);
  Stylesheet::process(s, context);

  // post processing
  extensions = &((ProcessingContext*)context)->getExtensions();

  for (e_it = extensions->begin(); e_it != extensions->end(); e_it++) {
    if ((*e_it).isAll()) {
      // go through all rulesets and look for occurances of the extension target.
    } else {
      // look for a ruleset that matches the extension target
      s.find((*e_it).getTarget());
    
      selector = new Selector((*r_it)->getSelector());
      (*e_it).updateSelector(*selector);
      (*r_it)->setSelector(*selector);
    }
  }
}

