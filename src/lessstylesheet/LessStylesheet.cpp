#include "LessStylesheet.h"
#include "LessMediaQuery.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

LessStylesheet::LessStylesheet() {
}

LessStylesheet::~LessStylesheet() {
}

LessRuleset* LessStylesheet::createLessRuleset() {
  LessRuleset* r = new LessRuleset();
  
#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating LessRuleset";
#endif
  
  addRuleset(*r);
  lessrulesets.push_back(r);
  r->setLessStylesheet(*this);
  return r;
}

Mixin* LessStylesheet::createMixin() {
  Mixin* m = new Mixin();
  
#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating mixin";
#endif
  
  addStatement(*m);
  m->setLessStylesheet(*this);
  return m;
}

LessAtRule* LessStylesheet::createLessAtRule(const Token &keyword) {
  LessAtRule* atrule = new LessAtRule(keyword);
  addAtRule(*atrule);
  atrule->setLessStylesheet(*this);
  return atrule;
}

LessMediaQuery* LessStylesheet::createLessMediaQuery() {
  LessMediaQuery* q = new LessMediaQuery();
  
#ifdef WITH_LIBGLOG
  VLOG(3) << "Adding Media Query";
#endif
  
  addStatement(*q);
  q->setLessStylesheet(*this);
  return q;
}

void LessStylesheet::deleteLessRuleset(LessRuleset &ruleset) {
  lessrulesets.remove(&ruleset);
  deleteStatement(ruleset);
}

void LessStylesheet::deleteMixin(Mixin &mixin) {
  deleteStatement(mixin);
}
  
void LessStylesheet::getFunctions(std::list<const Function*> &rulesetList,
                                  const Mixin &mixin) const {
  std::list<LessRuleset*>::const_iterator i;
  
  for (i = lessrulesets.cbegin(); i != lessrulesets.cend();
       i++) {

    (*i)->getFunctions(rulesetList, mixin, mixin.name.begin());
  }
}

void LessStylesheet::setContext(ProcessingContext* context) {
  this->context = context;
}
ProcessingContext* LessStylesheet::getContext() {
  return context;
}

void LessStylesheet::putVariable(const std::string &key, const TokenList &value) {
  variables[key] = value;
}
const TokenList* LessStylesheet::getVariable(const std::string &key) const {
  return variables.getVariable(key);
}



void LessStylesheet::process(Stylesheet &s, ProcessingContext &context) {
  std::list<Extension>* extensions;
  
  std::list<Ruleset*>::iterator r_it;
  std::list<Extension>::iterator e_it;
  std::list<Closure*> closureScope;
  
  this->context = &context;

  context.setLessStylesheet(*this);
  Stylesheet::process(s);
  
  saveReturnValues(context);
  
  // post processing
  extensions = &context.getExtensions();

  for (r_it = s.getRulesets().begin();
       r_it != s.getRulesets().end();
       r_it++) {
    for (e_it = extensions->begin();
         e_it != extensions->end();
         e_it++) {
      (*e_it).updateSelector((*r_it)->getSelector());
    }
  }
}

void LessStylesheet::saveReturnValues(ProcessingContext &context) {
  // move closures from context to this->closures
  context.saveClosures(this->closures);
  
  // move variables from context to this->variables
  context.saveVariables(this->variables);
}
