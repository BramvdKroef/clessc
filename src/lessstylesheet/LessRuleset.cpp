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

#include "LessRuleset.h"
#include "LessStylesheet.h"
#include "MediaQueryRuleset.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

LessRuleset::LessRuleset() : Ruleset() {
  parent = NULL;
  lessStylesheet = NULL;
  selector = NULL;
}
LessRuleset::LessRuleset(const Selector &selector) : Ruleset() {
  parent = NULL;
  lessStylesheet = NULL;
  setSelector(selector);
}
LessRuleset::~LessRuleset() {
  while (!nestedRules.empty()) {
    delete nestedRules.back();
    nestedRules.pop_back();
  }
  unprocessedStatements.clear();
  if (selector != NULL)
    delete selector;
}

void LessRuleset::setSelector(const Selector &selector) {
  this->selector = new LessSelector(selector);
  Ruleset::setSelector(*this->selector);
}
LessSelector* LessRuleset::getLessSelector() const {
  return selector;
}

UnprocessedStatement* LessRuleset::createUnprocessedStatement() {
  UnprocessedStatement* s = new UnprocessedStatement();
  
  Ruleset::addStatement(*s);
  s->setLessRuleset(*this);
  unprocessedStatements.push_back(s);
  return s;
}

const std::list<UnprocessedStatement*>&
  LessRuleset::getUnprocessedStatements() const {
  return unprocessedStatements;
}

LessRuleset* LessRuleset::createNestedRule() {
  LessRuleset* r = new LessRuleset();
  
#ifdef WITH_LIBGLOG
  VLOG(2) << "Creating nested rule" ;
#endif
  
  nestedRules.push_back(r);
  r->setParent(this);
  r->setLessStylesheet(*getLessStylesheet());
  return r;
}

MediaQueryRuleset* LessRuleset::createMediaQuery() {
  MediaQueryRuleset* r = new MediaQueryRuleset();
  
#ifdef WITH_LIBGLOG
  VLOG(2) << "Creating nested media query";
#endif
  
  nestedRules.push_back(r);
  r->setParent(this);
  r->setLessStylesheet(*getLessStylesheet());
  return r;
}

void LessRuleset::deleteNestedRule(LessRuleset &ruleset) {
  nestedRules.remove(&ruleset);
  delete &ruleset;
}

void LessRuleset::deleteUnprocessedStatement(UnprocessedStatement
                                             &statement) {
  unprocessedStatements.remove(&statement);
  deleteStatement(statement);
}

const std::list<LessRuleset*>& LessRuleset::getNestedRules() const {
  return nestedRules;
}

void LessRuleset::putVariable(const std::string &key, const TokenList &value) {
  variables[key] = value;  
}

map<string, TokenList>& LessRuleset::getVariables() {
  return variables;
}
 
void LessRuleset::setParent(LessRuleset* r) {
  parent = r;
}
LessRuleset* LessRuleset::getParent() const {
  return parent;
}

void LessRuleset::setLessStylesheet(LessStylesheet &s) {
#ifdef WITH_LIBGLOG
  VLOG(3) << "set LessStylesheet";
#endif
  lessStylesheet = &s;
}

LessStylesheet* LessRuleset::getLessStylesheet() const {
  return lessStylesheet;
}

ProcessingContext* LessRuleset::getContext() {
  return getLessStylesheet()->getContext();
}

void LessRuleset::processExtensions(ProcessingContext& context,
                                    Selector* prefix) {
  
  std::list<Extension>& e = getLessSelector()->getExtensions();
  std::list<Extension>::iterator e_it;
  Extension extension;

  for (e_it = e.begin(); e_it != e.end(); e_it++) {
    extension = *e_it;
    if (prefix != NULL) 
      extension.getExtension().addPrefix(*prefix);
    context.addExtension(extension);
  }
}

bool LessRuleset::insert(Mixin *mixin, Ruleset &target,
                         ProcessingContext &context) const {
  map<string, TokenList> scope;
  bool ret = false;
  context.pushFunction(*this);
  
  context.pushScope(scope);
  
  if (((mixin == NULL && !selector->needsArguments()) ||
       (mixin != NULL && putArguments(*mixin, scope))) &&
      matchConditions(context)) {

#ifdef WITH_LIBGLOG
    VLOG(2) << "Inserting variables";
#endif
    
    // set local variables
    context.pushScope(variables);

#ifdef WITH_LIBGLOG
    VLOG(2) << "Inserting statements";
#endif

    // process statements
    Ruleset::insert(target);

#ifdef WITH_LIBGLOG
    VLOG(2) << "Inserting nested rules";
#endif
  
    // insert nested rules
    insertNestedRules(*target.getStylesheet(), &target.getSelector(), context);

    if (mixin != NULL) {
      
#ifdef WITH_LIBGLOG
    VLOG(2) << "Adding closures";
#endif
      addClosures(context);
    }

    context.popScope();
    ret = true;
  }
  context.popScope();
  context.popFunction();
  return ret;
}

bool LessRuleset::insert(Mixin *mixin, Stylesheet &s,
                         ProcessingContext &context) const {
  map<string, TokenList> scope;
  const list<UnprocessedStatement*>& unprocessedStatements = getUnprocessedStatements();
  list<UnprocessedStatement*>::const_iterator up_it;
  bool ret = false;

  context.pushFunction(*this);
  context.pushScope(scope);

  if (((mixin == NULL && !selector->needsArguments()) ||
       (mixin != NULL && putArguments(*mixin, scope))) &&
      matchConditions(context)) {

    // set local variables
    context.pushScope(variables);
 
    // insert mixins
    for (up_it = unprocessedStatements.cbegin();
         up_it != unprocessedStatements.cend();
         up_it++) {
      (*up_it)->insert(s);
    }
    
    // insert nested rules
    insertNestedRules(s, NULL, context);

    if (mixin != NULL) {
      addClosures(context);
    }

    context.popScope();
    ret = true;
  }

  context.popScope();
  context.popFunction();
  return ret;
}

void LessRuleset::process(Stylesheet &s) {
  process(s, NULL, *getLessStylesheet()->getContext());
}
void LessRuleset::process(Stylesheet &s, Selector* prefix,
                          ProcessingContext& context) {
  Ruleset* target;
  std::list<Closure*> closureScope;

  if (selector->needsArguments())
    return;
  
  target = s.createRuleset();
  target->setSelector(getSelector());

  if (prefix != NULL)
    target->getSelector().addPrefix(*prefix);

#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing Less Ruleset: " <<
    getSelector().toString();
#endif

  context.interpolate(target->getSelector());

  processExtensions(context, prefix);

  context.pushClosureScope(closureScope);
  insert(NULL, *target, context);
  context.popClosureScope();
}

void LessRuleset::getFunctions(list<const Function*> &functionList,
                               const Mixin &mixin,
                               TokenList::const_iterator offset)
  const {

  const list<LessRuleset*>& nestedRules = getNestedRules();
  list<LessRuleset*>::const_iterator r_it;

  offset = mixin.name.walk(getSelector(), offset);
  
  if (offset == mixin.name.begin())
    return;

#ifdef WITH_LIBGLOG
  VLOG(3) << "Matching mixin " << mixin.name.toString() <<
    " against " << getSelector().toString();
#endif
  
  while (offset != mixin.name.end() &&
         ((*offset).type == Token::WHITESPACE ||
          *offset == ">")) {
    offset++;
  }
  
  if (offset == mixin.name.end()) {
    if (selector->matchArguments(mixin))
      functionList.push_back(this);

  } else {   
    for (r_it = nestedRules.cbegin(); r_it != nestedRules.cend(); r_it++) {
      (*r_it)->getFunctions(functionList, mixin, offset);
    }
  }
}

void LessRuleset::getLocalFunctions(std::list<const Function*> &functionList,
                                    const Mixin &mixin) const {
  const std::list<LessRuleset*>& nestedRules = getNestedRules();
  std::list<LessRuleset*>::const_iterator r_it;
  
  for (r_it = nestedRules.cbegin(); r_it != nestedRules.cend(); r_it++) {
    (*r_it)->getFunctions(functionList, mixin, mixin.name.begin());
  }

  if (getParent() != NULL) {
    getParent()->getLocalFunctions(functionList, mixin);
  } else {
    getLessStylesheet()->getFunctions(functionList, mixin);
  }
}


void LessRuleset::insertNestedRules(Stylesheet &s, Selector *prefix,
                                    ProcessingContext &context) const {
  const std::list<LessRuleset*>& nestedRules = getNestedRules();
  std::list<LessRuleset*>::const_iterator r_it;

  for (r_it = nestedRules.cbegin(); r_it != nestedRules.cend(); r_it++) {
    (*r_it)->process(s, prefix, context);
  }
}

void LessRuleset::addClosures(ProcessingContext &context) const {
  const std::list<LessRuleset*>& nestedRules = getNestedRules();
  std::list<LessRuleset*>::const_iterator r_it;
  
  for (r_it = nestedRules.cbegin(); r_it != nestedRules.cend(); r_it++) {
    if ((*r_it)->selector->needsArguments()) {
      context.addClosure(*(*r_it));
    }
  }
}


bool LessRuleset::matchConditions(ProcessingContext &context) const {
  std::list<TokenList>& conditions = selector->getConditions();
  std::list<TokenList>::iterator cit;
  TokenList condition;

  if (conditions.empty())
    return true;

  for(cit = conditions.begin(); cit != conditions.end(); cit++) {
    condition = (*cit);
    
#ifdef WITH_LIBGLOG
    VLOG(3) << "Checking condition: " << condition.toString();
#endif
    
    if (context.validateCondition(condition)) {
      
#ifdef WITH_LIBGLOG
      VLOG(3) << "Found valid condition: " << condition.toString();
#endif
      
      return true;
    }
  }
  return false;
}
  
bool LessRuleset::putArguments(const Mixin &mixin,
                               std::map<std::string, TokenList>
                               &scope) const {
  std::list<std::string>& parameters = selector->getParameters();
  std::list<std::string>::iterator pit;
  TokenList argsCombined;
  TokenList restVar;
  const TokenList* variable;
  size_t pos = 0;

  // combine with parameter names and add to local scope
  for(pit = parameters.begin(); pit != parameters.end(); pit++) {
    variable = mixin.getArgument(*pit);

    if (variable == NULL) 
      variable = mixin.getArgument(pos++);

    if (variable == NULL)
      variable = selector->getDefault(*pit);

    if (variable == NULL || variable->empty()) 
      return false;
    
    scope.insert(pair<std::string, TokenList>(*pit, *variable));

    argsCombined.insert(argsCombined.end(),
                        variable->begin(), variable->end());
    argsCombined.push_back(Token::BUILTIN_SPACE);
  }

  argsCombined.trim();

#ifdef WITH_LIBGLOG
  VLOG(3) << "@arguments: " << argsCombined.toString();
#endif

  if (selector->unlimitedArguments() &&
      selector->getRestIdentifier() != "") {

    while (pos < mixin.getArgumentCount()) {
      variable = mixin.getArgument(pos++);
      restVar.insert(restVar.end(),
                     variable->begin(), variable->end());
      restVar.push_back(Token::BUILTIN_SPACE);
    }
    
    restVar.trim();
    scope.insert(pair<std::string,TokenList>(selector->getRestIdentifier(), restVar));
  }
  
  scope.insert(pair<std::string,TokenList>("@arguments", argsCombined));
  return true;
}
