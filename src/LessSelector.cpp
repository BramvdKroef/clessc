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

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

LessSelector::LessSelector(Selector* original) {
  list<Selector*>* parts = original->split();
  list<Selector*>::iterator it;
  Selector* old_selector,
    *new_selector = new Selector();
  TokenList* extension;

  _needsArguments = false;
  _unlimitedArguments = false;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parsing less selector";
#endif
  
  for (it = parts->begin(); it != parts->end(); it++) {
    old_selector = *it;

    while(!old_selector->empty()) {
      extension = parseExtension(old_selector);
      if (extension != NULL) {
        extensions.insert(pair<string,TokenList*>
                          (extension->toString(), new_selector->clone()));
        delete extension;
      } else if (parts->size() == 1 &&
                 !new_selector->empty() &&
                 new_selector->back()->str != "nth-child" &&
                 parseArguments(old_selector)) {
        _needsArguments = true;
        old_selector->ltrim();
        
        parseConditions(old_selector);
      } else
        new_selector->push(old_selector->shift());
    }
    if (!empty()) 
      push(new Token(",", Token::OTHER));

    new_selector->trim();
    while(!new_selector->empty()) 
      push(new_selector->shift());
  }

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parsed selector: " << toString();
#endif
  
  delete new_selector;
}

LessSelector::~LessSelector() {
  map<string, TokenList*>::iterator i;
  
  while (!defaults.empty()) {
    delete defaults.back();
    defaults.pop_back();
  }
  while (!conditions.empty()) {
    delete conditions.back();
    conditions.pop_back();
  }
  for (i = extensions.begin();
       i != extensions.end();
       i++) {
    delete i->second;
  }
}

TokenList* LessSelector::parseExtension(TokenList* selector) {
  TokenList* extension;
  int parentheses = 1;
  
  if (selector->size() < 3 ||
      selector->front()->type != Token::COLON ||
      selector->at(1)->type != Token::IDENTIFIER ||
      selector->at(1)->str != "extend" ||
      selector->at(2)->type != Token::PAREN_OPEN)
    return NULL;

  extension = new TokenList();

  delete selector->shift(); // :
  delete selector->shift(); // extend
  delete selector->shift(); // (

  while(!selector->empty() && parentheses > 0) {
    if (selector->front()->type == Token::PAREN_OPEN) 
      parentheses++;
    else if (selector->front()->type == Token::PAREN_CLOSED)
      parentheses--;
    
    if (parentheses > 0)
      extension->push(selector->shift());
  }
  if (!selector->empty())
    delete selector->shift();

#ifdef WITH_LIBGLOG
  VLOG(2) << "Extension: " << extension->toString();
#endif
  
  return extension;  
}

bool LessSelector::parseArguments(TokenList* selector) {
  string delimiter;

  if (selector->front()->type != Token::PAREN_OPEN)
    return false;

  if (selector->contains(Token::DELIMITER, ";"))
    delimiter = ";";
  else
    delimiter = ",";

#ifdef WITH_LIBGLOG
  VLOG(3) << "Parameter delimiter: " << delimiter;
#endif
  
  if (!validateArguments(selector, delimiter))
    return false;

  delete selector->shift();

  selector->ltrim();

  while (parseParameter(selector, delimiter)) {
    selector->ltrim();
  }

  if (selector->size() > 3  &&
      selector->front()->str == "." &&
      selector->at(1)->str == "." &&
      selector->at(2)->str == ".") {
    _unlimitedArguments = true;
    delete selector->shift();
    delete selector->shift();
    delete selector->shift();
  }

  selector->ltrim();

  if (selector->front()->type != Token::PAREN_CLOSED) {
    throw new ParseException(selector->toString(),
                             "matching parentheses.", 0, 0, "");
  }

#ifdef WITH_LIBGLOG
  VLOG(3) << "Done parsing parameters";
#endif
  
  delete selector->shift();
  return true;
}


bool LessSelector::validateArguments(TokenList* arguments, string delimiter) {
  TokenListIterator* i = arguments->iterator();

  if (i->next()->type != Token::PAREN_OPEN)
    return false;

  while(i->hasNext() && i->next()->type == Token::WHITESPACE) {
  }

  while(i->hasNext()) {
    if (i->current()->type == Token::IDENTIFIER) {
      // switch
      i->next();
      
    } else if (i->current()->type == Token::ATKEYWORD) {
      // variable
      i->next();
      
      if (i->current()->type == Token::COLON) {
        // default value
        while (i->hasNext() &&
               i->next()->type != Token::PAREN_CLOSED &&
               i->current()->str != delimiter) {
        }
      
      } else if (i->current()->str == ".") {
        // rest
        if (!i->hasNext() || i->next()->str != "." ||
            !i->hasNext() || i->next()->str != ".") {
          delete i;
          return false;
        }
        break;
      }
    } else {
      break;
    }
    
    if (i->current()->str != delimiter)
      break;
    i->next();
    
    while(i->hasNext() && i->current()->type == Token::WHITESPACE) {
      i->next();
    }
  }
  
  while(i->hasNext() && i->current()->type == Token::WHITESPACE) {
    i->next();
  }

  // rest
  if (i->current()->str == ".") {
    if (!i->hasNext() || i->next()->str != "." ||
        !i->hasNext() || i->next()->str != ".") {
      delete i;
      return false;
    }
    if (i->hasNext())
      i->next();
  }
  if (i->current()->type != Token::PAREN_CLOSED) {
    delete i;
    return false;
  }

#ifdef WITH_LIBGLOG
  VLOG(2) << "Validated parameters";
#endif
  
  return true;
}

bool LessSelector::parseParameter(TokenList* selector, string delimiter) {
  string keyword;
  TokenList* value = NULL;

  if (selector->empty())
    return false;

  if (selector->front()->type == Token::IDENTIFIER) {
    keyword = selector->front()->str;
    delete selector->shift();

  } else if (selector->front()->type == Token::ATKEYWORD) {

    keyword = selector->front()->str;
    delete selector->shift();


    if ((value = parseDefaultValue(selector, delimiter)) != NULL) {
      // default value
      
    } else if (selector->size() > 3 &&
               selector->front()->str == "." &&
               selector->at(1)->str == "." &&
               selector->at(2)->str == ".") {
      // rest argument
      delete selector->shift();
      delete selector->shift();
      delete selector->shift();
      
      restIdentifier = keyword;
      _unlimitedArguments = true;
      return true;
    }
  } else
    return false;

  selector->ltrim();
  
  if (!selector->empty() &&
      selector->front()->str == delimiter)
    delete selector->shift();

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parameeter: " << keyword;
#endif
  
  parameters.push_back(keyword);
  defaults.push_back(value);
  return true;
}

TokenList* LessSelector::parseDefaultValue(TokenList* arguments,
                                     string delimiter) {
  TokenList* value;
  int parentheses = 0;
  
  if (arguments->front()->type != Token::COLON)
    return NULL;
  
  delete arguments->shift();
  value = new TokenList();
    
  while (!arguments->empty() &&
         (parentheses != 0 ||
          arguments->front()->type != Token::PAREN_CLOSED) &&
         arguments->front()->str != delimiter) {

    if (arguments->front()->type == Token::PAREN_OPEN)
      parentheses++;
    if (arguments->front()->type == Token::PAREN_CLOSED)
      parentheses--;

    value->push(arguments->shift());
  }

  value->trim();

  if (value->empty()) {
    throw new ParseException("",
                             "default value following ':'",
                             0, 0, "");
  }
  return value;
}

bool LessSelector::parseConditions (TokenList* selector) {
  TokenList* condition;
  
  if (selector->empty() ||
      selector->front()->str != "when")
    return false;

#ifdef WITH_LIBGLOG
  VLOG(3) << "Parsing conditions";
#endif
  
  delete selector->shift();

  selector->ltrim();
  
  while (!selector->empty()) {
    condition = new TokenList();
    
    while(!selector->empty() && selector->front()->str != ",") {
      condition->push(selector->shift());
    }
    if (!selector->empty() && selector->front()->str == ",")
      delete selector->shift();

#ifdef WITH_LIBGLOG
    VLOG(2) << "Condition: " << condition->toString();
#endif
    
    conditions.push_back(condition);
  }
  return true;
}


TokenList* LessSelector::getDefault(string keyword) {
  list<string>::iterator pit = parameters.begin();
  list<TokenList*>::iterator dit = defaults.begin();
  for (;pit != parameters.end(); pit++, dit++) {
    if ((*pit) == keyword)
      return (*dit);
  }
  return NULL;
}

list<string>* LessSelector::getParameters() {
  return &parameters;
}

list<TokenList*>* LessSelector::getConditions() {
  return &conditions;
}

map<string, TokenList*>* LessSelector::getExtensions() {
  return &extensions;
}

bool LessSelector::matchArguments(ParameterMixin* mixin) {
  list<string>::iterator p_it = parameters.begin();
  list<TokenList*>::iterator d_it = defaults.begin();
  size_t pos = 0;

  for(; p_it != parameters.end(); p_it++, d_it++) {
    
    if (mixin->getArgument(*p_it) == NULL &&
        mixin->getArgument(pos++) == NULL &&
        *d_it == NULL) {
          return false;
    }
  }
  return (pos >= mixin->getArgumentCount() || unlimitedArguments());
}

bool LessSelector::needsArguments() {
  return _needsArguments;
}
bool LessSelector::unlimitedArguments() {
  return _unlimitedArguments;
}
string LessSelector::getRestIdentifier() {
  return restIdentifier;
}
