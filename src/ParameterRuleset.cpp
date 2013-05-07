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

ParameterRuleset::ParameterRuleset(Selector* selector): Ruleset(selector) {
  Selector* newselector = new Selector();
  
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
  if (selector->front()->type != Token::PAREN_CLOSED) {
    throw new ParseException(*selector->toString(),
                             "matching parentheses.");
  }
  delete selector;
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
  }
  if (!selector->empty() &&
      (selector->front()->str == "," ||
       selector->front()->str == ";")) {
    delete selector->shift();
  }
  
  addParameter(keyword, value);
  return true;
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
  return true;
}
bool ParameterRuleset::matchConditions(ValueProcessor* valueProcessor){
  (void)valueProcessor;
  return true;
}
  
bool ParameterRuleset::putArguments(ValueProcessor* valueProcessor,
                                    list<TokenList*>* arguments) {
  list<string> parameters = getKeywords();
  list<TokenList*>::iterator ait  = arguments->begin();
  list<string>::iterator pit = parameters.begin();
  TokenList* argsCombined = new TokenList();
  TokenList* variable;
  
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
  
  valueProcessor->putVariable("@arguments", argsCombined);
  return true;
}
void ParameterRuleset::addDeclarations(ValueProcessor* valueProcessor,
                                       Ruleset* ruleset) {
  vector<Declaration*>* declarations;
  vector<Declaration*>::iterator di;
  Declaration* declaration;

  declarations = getDeclarations();  
  for (di = declarations->begin(); di < declarations->end(); di++) {
    declaration = (*di)->clone();
    valueProcessor->processValue(declaration->getValue());
    ruleset->addDeclaration(declaration);
  }
}
