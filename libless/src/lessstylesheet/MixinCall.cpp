#include "less/lessstylesheet/MixinCall.h"
#include "less/lessstylesheet/Function.h"
#include "less/lessstylesheet/Mixin.h"
#include "less/lessstylesheet/ProcessingContext.h"

MixinCall::MixinCall(MixinCall* parent,
                     const Function& function,
                     bool savepoint,
                     bool important) :
  parent(parent), function(&function), savepoint(savepoint) {
  
  this->important = important || (parent != NULL && parent->important);
}

const TokenList* MixinCall::getVariable(const std::string& key,
                                        const ProcessingContext& context) const {
  const TokenList* t;
    
  if ((t = function->getVariable(key, context)) != NULL)
    return t;

  if (parent != NULL)
    return parent->getVariable(key, context);
  return NULL;
}

void MixinCall::getFunctions(std::list<const Function*>& functionList,
                             const Mixin& mixin,
                             const ProcessingContext &context) const {
  function->getLocalFunctions(functionList, mixin, context);
  if (!functionList.empty())
    return;

  if (parent != NULL)
    parent->getFunctions(functionList, mixin, context);
}

bool MixinCall::isInStack(const Function& function) const {
  return (this->function == &function) ||
         (parent != NULL && parent->isInStack(function));
}

const VariableMap* MixinCall::getArguments(const Function& function) const {
  if (this->function == &function)
    return &arguments;

  if (parent != NULL)
    return parent->getArguments(function);

  return NULL;
}
