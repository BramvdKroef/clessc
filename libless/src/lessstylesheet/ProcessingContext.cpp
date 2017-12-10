#include <less/lessstylesheet/ProcessingContext.h>
#include <less/lessstylesheet/MixinCall.h>
#include <less/lessstylesheet/LessRuleset.h>
#include <less/lessstylesheet/LessStylesheet.h>

ProcessingContext::ProcessingContext() {
  stack = NULL;
  contextStylesheet = NULL;
}

void ProcessingContext::setLessStylesheet(LessStylesheet &stylesheet) {
  contextStylesheet = &stylesheet;
}
LessStylesheet* ProcessingContext::getLessStylesheet() {
  return contextStylesheet;
}

const TokenList* ProcessingContext::getVariable(const std::string &key) const {
  if (stack != NULL)
    return stack->getVariable(key);
  else if (contextStylesheet != NULL)
    return contextStylesheet->getVariable(key);
  else
    return NULL;
}

void ProcessingContext::pushMixinCall(const Function &function, bool
  savepoint) {
  stack = new MixinCall(stack, function, savepoint);
}

void ProcessingContext::popMixinCall() {
  MixinCall* tmp = stack;
  if (stack != NULL) {
    stack = stack->parent;

    if (closures.empty() && variables.empty())
      delete tmp;
  }
}

VariableMap* ProcessingContext::getStackArguments() {
  if (stack != NULL)
    return &stack->arguments;
  else
    return NULL;
}

bool ProcessingContext::isStackEmpty() const {
  return stack == NULL;
}
bool ProcessingContext::isSavePoint() const {
  return (stack != NULL && stack->savepoint);
}

void ProcessingContext::getFunctions (std::list<const Function*> &functionList,
                                      const Mixin& mixin) const {
  getClosures(functionList, mixin);

  if (stack != NULL) 
    stack->getFunctions(functionList, mixin);
  else if (contextStylesheet != NULL)
    contextStylesheet->getFunctions(functionList, mixin);
}

bool ProcessingContext::isInStack(const Function &function) const {
  if (stack != NULL)
    return stack->isInStack(function);
  else
    return NULL;
}

void ProcessingContext::addExtension(Extension& extension){
  extensions.push_back(extension);
}
std::list<Extension>& ProcessingContext::getExtensions() {
  return extensions;
}

void ProcessingContext::addClosure(const LessRuleset &ruleset) {
  if (stack != NULL) {
    Closure* c = new Closure(ruleset, *stack);
    closures.push_back(c);
  }
}
void ProcessingContext::saveClosures(std::list<Closure*> &closures) {
  closures.insert(closures.end(), this->closures.begin(), this->closures.end());
  this->closures.clear();
}

void ProcessingContext::addVariables(const VariableMap &variables) {
  this->variables.overwrite(variables);
}
void ProcessingContext::saveVariables(VariableMap &variables) {
  variables.merge(this->variables);
  this->variables.clear();
}

void ProcessingContext::getClosures(std::list<const Function*> &closureList,
                                        const Mixin &mixin) const {
  std::list<Closure*>::const_iterator it;
  
  for (it = closures.begin(); it != closures.end(); it++) {
    (*it)->getFunctions(closureList, mixin, mixin.name.begin());
  }
}

ValueProcessor* ProcessingContext::getValueProcessor() {
  return &processor;
}

void ProcessingContext::interpolate(TokenList &tokens) {
  processor.interpolate(tokens, *this);
}
void ProcessingContext::interpolate(std::string &str) {
  processor.interpolate(str, *this);
}

void ProcessingContext::processValue(TokenList& value) {
  processor.processValue(value, *this);
}

bool ProcessingContext::validateCondition(TokenList &value) {
  return processor.validateCondition(value, *this);
}
