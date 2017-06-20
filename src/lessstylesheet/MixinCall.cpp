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

#include "MixinCall.h"
#include "Function.h"
#include "Mixin.h"

MixinCall::MixinCall(MixinCall* parent, const Function& function) {
  this->parent = parent;
  this->function = &function;
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
