#include "Closure.h"
#include "ProcessingContext.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

Closure::Closure() {
  this->ruleset = NULL;
}
Closure::Closure(const LessRuleset &ruleset) {
  this->ruleset = &ruleset;
}

bool Closure::insert(Mixin* mixin, Ruleset &target,
                    ProcessingContext& context) const {
  bool ret;
  
  context.pushScope(variables);

#ifdef WITH_LIBGLOG
  std::map<std::string, TokenList>::const_iterator i;
  
  for (i = variables.cbegin(); i != variables.cend(); ++i)
    VLOG(3) << i->first << " => " << i->second.toString();
#endif
  
  ret = ruleset->insert(mixin, target, context);
  context.popScope();
  return ret;
}
  
bool Closure::insert(Mixin* mixin, Stylesheet &s,
                    ProcessingContext& context) const {
  bool ret;
  context.pushScope(variables);
  ret = ruleset->insert(mixin, s, context);
  context.popScope();
  return ret;
}
  

void Closure::getFunctions(list<const Function*> &functionList,
                  const Mixin &mixin,
                  TokenList::const_iterator offset) const {

  const list<LessRuleset*>& nestedRules = ruleset->getNestedRules();
  list<LessRuleset*>::const_iterator r_it;

  offset = mixin.name.walk(ruleset->getSelector(), offset);
  
  if (offset == mixin.name.begin())
    return;

#ifdef WITH_LIBGLOG
  VLOG(3) << "Matching mixin " << mixin.name.toString() <<
    " against closure " << ruleset->getSelector().toString();
#endif
  
  while (offset != mixin.name.end() &&
         ((*offset).type == Token::WHITESPACE ||
          *offset == ">")) {
    offset++;
  }
  
  if (offset == mixin.name.end()) {
    if (ruleset->getLessSelector()->matchArguments(mixin))
      functionList.push_back(this);

  } else {   
    for (r_it = nestedRules.cbegin(); r_it != nestedRules.cend(); r_it++) {
      (*r_it)->getFunctions(functionList, mixin, offset);
    }
  }
}

LessSelector* Closure::getLessSelector() const {
  return ruleset->getLessSelector();
}
