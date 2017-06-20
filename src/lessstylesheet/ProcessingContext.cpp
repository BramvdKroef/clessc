/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

#include "ProcessingContext.h"
#include "MixinCall.h"
#include "LessRuleset.h"
#include "LessStylesheet.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif


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

void ProcessingContext::pushMixinCall(const Function &function) {
  stack = new MixinCall(stack, function);
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
  
  for (it = closures.cbegin(); it != closures.cend(); it++) {
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
