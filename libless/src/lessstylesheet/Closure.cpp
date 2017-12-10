#include "less/lessstylesheet/Closure.h"
#include "less/LogStream.h"
#include "less/lessstylesheet/ProcessingContext.h"

Closure::Closure(const LessRuleset& ruleset, const MixinCall& stack) {
  this->ruleset = &ruleset;
  this->stack = &stack;
}

bool Closure::call(Mixin& mixin,
                   Ruleset& target,
                   ProcessingContext& context) const {
  return ruleset->call(mixin, target, context);
}

bool Closure::call(Mixin& mixin,
                   Stylesheet& s,
                   ProcessingContext& context) const {
  return ruleset->call(mixin, s, context);
}

void Closure::getFunctions(list<const Function*>& functionList,
                           const Mixin& mixin,
                           TokenList::const_iterator offset) const {
  const list<LessRuleset*>& nestedRules = ruleset->getNestedRules();
  const list<Closure*>& closures = ruleset->getClosures();
  list<LessRuleset*>::const_iterator r_it;
  list<Closure*>::const_iterator c_it;

  offset = mixin.name.walk(*getLessSelector(), offset);

  if (offset == mixin.name.begin())
    return;

  LogStream().notice(3) << "Matching mixin " << mixin.name.toString()
                        << " against " << getLessSelector()->toString();

  while (offset != mixin.name.end() &&
         ((*offset).type == Token::WHITESPACE || *offset == ">")) {
    offset++;
  }

  if (getLessSelector()->matchArguments(mixin)) {
    if (offset == mixin.name.end()) {
      functionList.push_back(this);
    } else {
      for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
        (*r_it)->getFunctions(functionList, mixin, offset);
      }
      for (c_it = closures.begin(); c_it != closures.end(); c_it++) {
        (*c_it)->getFunctions(functionList, mixin, mixin.name.begin());
      }
    }
  }
}

LessSelector* Closure::getLessSelector() const {
  return ruleset->getLessSelector();
}

const TokenList* Closure::getVariable(const std::string& key) const {
  return ruleset->getVariable(key);
}

const TokenList* Closure::getInheritedVariable(const std::string& key,
                                               const MixinCall& stack) const {
  const TokenList* t;

  t = ruleset->getInheritedVariable(key, *this->stack);
  if (t == NULL)
    t = this->stack->getVariable(key);
  return t;
}

void Closure::getLocalFunctions(std::list<const Function*>& functionList,
                                const Mixin& mixin) const {
  ruleset->getFunctions(functionList, mixin, mixin.name.begin());
  if (functionList.empty())
    stack->getFunctions(functionList, mixin);
}
