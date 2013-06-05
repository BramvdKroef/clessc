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

#include "ParameterRuleset.h"
#include <iostream>

bool ParameterRuleset::isValid(Selector* selector) {
  TokenListIterator* it = selector->iterator();
  while (it->hasNext() &&
         it->next()->type != Token::PAREN_OPEN) {
  }
  
  if (!it->hasNext()) {
    delete it;
    return false;
  }

  while(it->hasNext() && it->next()->type == Token::WHITESPACE) {
  }

  while (it->current()->type == Token::ATKEYWORD) {
    if (!it->hasNext())
      break;

    if (it->next()->type == Token::COLON) {
      while (it->hasNext() &&
             it->next()->type != Token::PAREN_CLOSED &&
             it->current()->str != "," &&
             it->current()->str != ";") {
      }
      
    } else if (it->current()->str == ".") {
      if (!it->hasNext() || it->next()->str != "." ||
          !it->hasNext() || it->next()->str != ".") {
        delete it;
        return false;
      }
      while(it->hasNext() && it->next()->type == Token::WHITESPACE) {
      }
      break;
    }
    
    if (it->current()->str != "," &&
        it->current()->str != ";") {
      break;
    }

    while(it->hasNext() && it->next()->type == Token::WHITESPACE) {
    }
  }
  
  if (it->current()->str == ".") {
    if (!it->hasNext() || it->next()->str != "." ||
        !it->hasNext() || it->next()->str != ".") {
      delete it;
      return false;
    }
    if (it->hasNext())
      it->next();
  }
  if (it->current()->type != Token::PAREN_CLOSED) {
    delete it;
    return false;
  }

  while(it->hasNext() && it->next()->type == Token::WHITESPACE) {
  }
  if (it->current()->str == "when") {
    while (it->hasNext()) {
      it->next();
    }
  }
  
  delete it;
  return true;
}

ParameterRuleset::ParameterRuleset(Selector* selector): Ruleset(selector) {
  Selector* newselector = new Selector();
  rest = "";
  unlimitedArguments = false;
  
  while (!selector->empty() &&
         selector->front()->type != Token::PAREN_OPEN) {
    newselector->push(selector->shift());
  }
  if (selector->empty()) {
    throw new ParseException(*selector->toString(),
                             "matching parentheses.");
  }
  
  delete selector->shift();
  while (newselector->back()->type == Token::WHITESPACE) 
    delete newselector->pop();
  setSelector(newselector);
  
  while (processParameter(selector)) {
  }
  if (selector->size() > 3  &&
      selector->front()->str == "." &&
      selector->at(1)->str == "." &&
      selector->at(2)->str == ".") {
    unlimitedArguments = true;
    delete selector->shift();
    delete selector->shift();
    delete selector->shift();
  }

  if (selector->front()->type != Token::PAREN_CLOSED) {
    throw new ParseException(*selector->toString(),
                             "matching parentheses.");
  }
  delete selector->shift();
  
  while (!selector->empty() &&
         selector->front()->type == Token::WHITESPACE)
    delete selector->shift();
  
  if (!selector->empty() &&
      selector->front()->str == "when") {
    delete selector->shift();
    
    while (!selector->empty() &&
           selector->front()->type == Token::WHITESPACE)
      delete selector->shift();
    processConditions(selector);
  }
  delete selector;
}

ParameterRuleset::~ParameterRuleset() {
  while (!defaults.empty()) {
    delete defaults.back();
    defaults.pop_back();
  }
  while (!conditions.empty()) {
    delete conditions.back();
    conditions.pop_back();
  }
  while (!nestedRules.empty()) {
    delete nestedRules.back();
    nestedRules.pop_back();
  }
  while (!mixins.empty()) {
    delete mixins.back();
    mixins.pop_back();
  }
}

void ParameterRuleset::addParameter(string keyword, TokenList* value) {
  parameters.push_back(keyword);
  defaults.push_back(value);
}

TokenList* ParameterRuleset::getDefault(string keyword) {
  list<string>::iterator pit = parameters.begin();
  list<TokenList*>::iterator dit = defaults.begin();
  for (;pit != parameters.end(); pit++, dit++) {
    if ((*pit) == keyword)
      return (*dit);
  }
  return NULL;
}

list<string> ParameterRuleset::getKeywords() {
  return parameters;
}

void ParameterRuleset::addCondition(TokenList* condition) {
  conditions.push_back(condition);
}

bool ParameterRuleset::processParameter(Selector* selector) {
  string keyword;
  TokenList* value = NULL;

  while (!selector->empty() &&
         selector->front()->type == Token::WHITESPACE) {
    delete selector->shift();
  }

  if (selector->empty() ||
      selector->front()->type != Token::ATKEYWORD)
    return false;

  keyword = selector->front()->str;
  delete selector->shift();
  
  if (selector->front()->type == Token::COLON) {
    delete selector->shift();
    value = new TokenList();
    
    while (!selector->empty() &&
           selector->front()->type != Token::PAREN_CLOSED &&
           selector->front()->str != "," &&
           selector->front()->str != ";") {
      value->push(selector->shift());
    }
    
    while (!value->empty() && value->front()->type ==
           Token::WHITESPACE) {
      delete value->shift();
    }
    if (value->empty()) {
      throw new ParseException("",
                               "default value following ':'");
    }
  } else if (selector->size() > 3 &&
             selector->front()->str == "." &&
             selector->at(1)->str == "." &&
             selector->at(2)->str == ".") {
    delete selector->shift();
    delete selector->shift();
    delete selector->shift();
    rest = keyword;
    unlimitedArguments = true;
    return true;
  }
  if (!selector->empty() &&
      (selector->front()->str == "," ||
       selector->front()->str == ";")) {
    delete selector->shift();
  }
  
  addParameter(keyword, value);
  return true;
}

void ParameterRuleset::processConditions(Selector* selector) {
  TokenList* condition;
  while (!selector->empty()) {
    condition = new TokenList();
    
    while(!selector->empty() && selector->front()->str != ",") {
      condition->push(selector->shift());
    }
    if (!selector->empty() && selector->front()->str == ",")
      delete selector->shift();
    
    addCondition(condition);
  }
}

bool ParameterRuleset::matchArguments(list<TokenList*>* arguments) {
  list<string> parameters = getKeywords();
  list<TokenList*>::iterator ait  = arguments->begin();
  list<string>::iterator pit = parameters.begin();

  for(; pit != parameters.end(); pit++) {
    if (ait != arguments->end()) 
      ait++;
    else if (getDefault(*pit) == NULL) 
      return false;
  }
  return (ait == arguments->end() || unlimitedArguments);
}

bool ParameterRuleset::matchConditions(ValueProcessor* valueProcessor){
  list<TokenList*>::iterator cit = conditions.begin();
  TokenList* condition;
  if (conditions.size() == 0)
    return true;
  
  for(; cit != conditions.end(); cit++) {
    condition = (*cit)->clone();
    
    if (valueProcessor->validateValue(condition)) {
      delete condition;
      return true;
    } else
      delete condition;
  }
  return false;
}
  
bool ParameterRuleset::putArguments(ValueProcessor* valueProcessor,
                                    list<TokenList*>* arguments) {
  list<string> parameters = getKeywords();
  list<TokenList*>::iterator ait  = arguments->begin();
  list<string>::iterator pit = parameters.begin();
  TokenList* argsCombined = new TokenList();
  TokenList* restVar = NULL;
  TokenList* variable;

  if (unlimitedArguments && rest != "")
    restVar = new TokenList();
  
  // combine with parameter names and add to local scope
  for(; pit != parameters.end(); pit++) {
    if (ait != arguments->end()) {
      valueProcessor->putVariable(*pit, *ait);
      argsCombined->push((*ait)->clone());
      ait++;
    } else {
      variable = getDefault(*pit);
      if (variable == NULL) {
        delete argsCombined;
        return false;
      }
      valueProcessor->putVariable(*pit, variable->clone());
      argsCombined->push(variable->clone());
    }
    argsCombined->push(new Token(" ", Token::WHITESPACE));
  }
  
  if (argsCombined->size() > 0)
    delete argsCombined->pop();

  if (restVar != NULL) {
    while (ait != arguments->end()) {
      restVar->push(*ait);
      restVar->push(new Token(" ", Token::WHITESPACE));
      ait++;
    }
    if (restVar->size() > 0)
      delete restVar->pop();
    valueProcessor->putVariable(rest, restVar);
  }
  
  valueProcessor->putVariable("@arguments", argsCombined);
  return true;
}


void ParameterRuleset::addNestedRule(Ruleset* nestedRule) {
  nestedRules.push_back(nestedRule);
}
list<Ruleset*>* ParameterRuleset::getNestedRules() {
  return &nestedRules;
}
void ParameterRuleset::addMixin(ParameterMixin* mixin) {
  mixins.push_back(mixin);
}
list<ParameterMixin*>* ParameterRuleset::getMixins() {
  return &mixins;
}
