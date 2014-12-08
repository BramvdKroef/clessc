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
#include "LessRuleset.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

ProcessingContext::ProcessingContext() {
  scopes = NULL;
}
  
const TokenList* ProcessingContext::getVariable(const std::string &key) {
  return scopes->getVariable(key);
}
void ProcessingContext::pushScope(const std::map<std::string,
                                  TokenList> &scope) {
  if (scopes == NULL)
    scopes = new ValueScope(scope);
  else
    scopes = new ValueScope(*scopes, scope);
}
void ProcessingContext::popScope() {
  const ValueScope* tmp = scopes;

  if (scopes == NULL)
    return;
  
  scopes = tmp->getParent();
  delete tmp;
}
  
void ProcessingContext::pushRuleset(const LessRuleset &ruleset) {
#ifdef WITH_LIBGLOG
  VLOG(2) << "Push: " << ruleset.getSelector().toString();
#endif
  
  rulesets.push_back(&ruleset);
}

void ProcessingContext::popRuleset() {
#ifdef WITH_LIBGLOG
  VLOG(2) << "Pop: " << rulesets.back()->getSelector().toString();
#endif

  rulesets.pop_back();
}

bool ProcessingContext::isInStack(const LessRuleset &ruleset) {
  std::list<const LessRuleset*>::iterator i;

  for(i = rulesets.begin(); i != rulesets.end(); i++) {
    if (*i == &ruleset)
      return true;
  }
  return false;
}

void ProcessingContext::addExtension(Extension& extension){
  extensions.push_back(extension);
}
std::list<Extension>& ProcessingContext::getExtensions() {
  return extensions;
}

ValueProcessor* ProcessingContext::getValueProcessor() {
  return &processor;
}

void ProcessingContext::interpolate(TokenList &tokens) {
  processor.interpolate(tokens, *scopes);
}
void ProcessingContext::interpolate(std::string &str) {
  processor.interpolate(str, *scopes);
}

void ProcessingContext::processValue(TokenList& value) {
  processor.processValue(value, *scopes);
}

bool ProcessingContext::validateCondition(TokenList &value) {
  return processor.validateCondition(value, *scopes);
}
