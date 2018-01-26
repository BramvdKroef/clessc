#include "less/lessstylesheet/Closure.h"
#include "less/lessstylesheet/ProcessingContext.h"
#include "less/lessstylesheet/LessRuleset.h"

Closure::Closure(const LessRuleset& ruleset, const MixinCall& stack) {
  this->ruleset = &ruleset;
  this->stack = &stack;
}

bool Closure::call(MixinArguments& args,
                   Ruleset& target,
                   ProcessingContext& context) const {
  return ruleset->call(args, target, context);
}

bool Closure::call(MixinArguments& args,
                   Stylesheet& s,
                   ProcessingContext& context) const {
  return ruleset->call(args, s, context);
}

void Closure::getFunctions(std::list<const Function*>& functionList,
                           const Mixin& mixin,
                           TokenList::const_iterator offset,
                           const ProcessingContext &context) const {
  const std::list<LessRuleset*>& nestedRules = ruleset->getNestedRules();
  const std::list<Closure*> *closures = context.getClosures(ruleset);
  std::list<LessRuleset*>::const_iterator r_it;
  std::list<Closure*>::const_iterator c_it;

  offset = mixin.name.walk(*getLessSelector(), offset);

  if (offset == mixin.name.begin())
    return;

  while (offset != mixin.name.end() &&
         ((*offset).type == Token::WHITESPACE || *offset == ">")) {
    offset++;
  }

  if (getLessSelector()->matchArguments(mixin.arguments)) {
    if (offset == mixin.name.end()) {
      functionList.push_back(this);
    } else {
      for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
        (*r_it)->getFunctions(functionList, mixin, offset, context);
      }
      if (closures != NULL) {
        for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
          (*c_it)->getFunctions(functionList, mixin, offset, context);
        }
      }
    }
  }
}

LessSelector* Closure::getLessSelector() const {
  return ruleset->getLessSelector();
}

const TokenList* Closure::getVariable(const std::string& key,
                                      const ProcessingContext &context) const {
  const TokenList* t;

  if ((t = ruleset->getVariable(key, context)) != NULL)
    return t;
  return this->stack->getVariable(key, context);  
}

void Closure::getLocalFunctions(std::list<const Function*>& functionList,
                                const Mixin& mixin,
                                const ProcessingContext &context) const {
  ruleset->getFunctions(functionList, mixin, mixin.name.begin(), context);
  if (functionList.empty())
    stack->getFunctions(functionList, mixin, context);
}
