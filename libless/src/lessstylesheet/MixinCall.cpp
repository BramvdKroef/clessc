
#include "MixinCall.h"
#include "Function.h"
#include "Mixin.h"

MixinCall::MixinCall(MixinCall* parent, const Function& function, bool
  savepoint) {
  this->parent = parent;
  this->function = &function;
  this->savepoint = savepoint;
}

const TokenList* MixinCall::getVariable(const std::string &key) const {
  VariableMap::const_iterator mit;
  const TokenList* t;
  
  if ((t = function->getVariable(key)) != NULL)
    return t;

  if ((t = arguments.getVariable(key)) != NULL)
    return t;
  
  if ((t = function->getInheritedVariable(key, *this)) != NULL)
    return t;

  if (parent != NULL)
    return parent->getVariable(key);
  return NULL;
}

void MixinCall::getFunctions (std::list<const Function*> &functionList,
                              const Mixin& mixin) const {

  function->getLocalFunctions(functionList, mixin);
  if (!functionList.empty())
    return;

  if (parent != NULL)
    parent->getFunctions(functionList, mixin);
}

bool MixinCall::isInStack(const Function &function) const {
  return (this->function == &function) ||
    (parent != NULL && parent->isInStack(function));
}

const VariableMap* MixinCall::getArguments(const Function &function) const {
  if (this->function == &function)
    return &arguments;
  
  if (parent != NULL)
    return parent->getArguments(function);
  
  return NULL;
}
