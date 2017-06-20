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

#ifndef __ProcessingContext_h__
#define __ProcessingContext_h__

#include <map>
#include <string>
#include <list>

#include "../TokenList.h"
#include "../VariableMap.h"
#include "../value/ValueScope.h"
#include "../value/ValueProcessor.h"
#include "../lessstylesheet/Extension.h"
#include "MixinCall.h"
#include "Function.h"
#include "Closure.h"

class LessRuleset;
class Function;
class Closure;
class Mixin;
class LessStylesheet;
  
class ProcessingContext: public ValueScope {
private:
  MixinCall* stack;

  ValueProcessor processor;
  std::list<Extension> extensions;

  LessStylesheet* contextStylesheet;
  
  // return values
  std::list<Closure*> closures;
  VariableMap variables;  

public:
  ProcessingContext();

  void setLessStylesheet(LessStylesheet &stylesheet);
  LessStylesheet* getLessStylesheet();
  
  virtual const TokenList* getVariable(const std::string &key) const;
  
  void pushMixinCall(const Function &function);
  void popMixinCall();
  bool isInStack(const Function &function) const;
  VariableMap* getStackArguments();
  bool isStackEmpty() const;

  void getFunctions (std::list<const Function*> &functionList,
                     const Mixin& mixin) const;
  void getClosures(std::list<const Function*> &closureList,
                   const Mixin &mixin) const;
  
  void saveClosures(std::list<Closure*> &closures);
  void saveVariables(VariableMap &variables);

  void addClosure(const LessRuleset &ruleset);
  void addVariables(const VariableMap &variables);
  
  void addExtension(Extension& extension);
  std::list<Extension>& getExtensions();

  ValueProcessor* getValueProcessor();

  void interpolate(TokenList &tokens);
  void interpolate(std::string &str);
  void processValue(TokenList& value);
  bool validateCondition(TokenList &value);
};

#endif
