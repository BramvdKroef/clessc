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

LessAtRule* LessStylesheet::createLessAtRule(std::string &keyword) {
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
  
void LessStylesheet::getLessRulesets(list<LessRuleset*> &rulesetList,
                                     const Mixin &mixin) {
  list<LessRuleset*>::iterator i;
  
  for (i = lessrulesets.begin(); i != lessrulesets.end();
       i++) {

    (*i)->getLessRulesets(rulesetList, mixin, mixin.name.begin());
  }
}

void LessStylesheet::setContext(ProcessingContext* context) {
  this->context = context;
}
ProcessingContext* LessStylesheet::getContext() {
  return context;
}

void LessStylesheet::putVariable(const std::string &key, const TokenList &value) {
  map<std::string, TokenList>::iterator mit;
  
  // check if variable is alread declared
  mit = variables.find(key);
  
  if (mit != variables.end()) {
#ifdef WITH_LIBGLOG
    LOG(WARNING) << "Variable " << key << " defined twice in same stylesheet.";
#else
    std::cerr << "Variable " << key << " defined twice in same stylesheet.\n";
#endif
  }
  
  variables.insert(std::pair<std::string, TokenList>(key, value));  
}


void LessStylesheet::process(Stylesheet &s, ProcessingContext &context) {
  std::list<Extension>* extensions;
  
  std::list<Ruleset*>::iterator r_it;
  std::list<Extension>::iterator e_it;

  context.pushScope(variables);

  this->context = &context;
  
  Stylesheet::process(s);

  context.popScope();


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
