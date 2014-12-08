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

LessSelector::LessSelector(const Selector &original) {
  list<Selector> parts;
  list<Selector>::iterator it;
  Selector* old_selector;
  Selector new_selector;

  original.split(parts);
  
  _needsArguments = false;
  _unlimitedArguments = false;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parsing less selector";
#endif
  
  for (it = parts.begin(); it != parts.end(); it++) {
    old_selector = &(*it);

    while(!old_selector->empty()) {
      
      if (parseExtension(*old_selector, new_selector)) {
        
      } else if (parts.size() == 1 &&
                 !new_selector.empty() &&
                 new_selector.back() != "nth-child" &&
                 parseArguments(*old_selector)) {
        _needsArguments = true;
        old_selector->ltrim();
        
        parseConditions(*old_selector);
        
      } else {
        new_selector.push_back(old_selector->front());
        old_selector->pop_front();
      }
      
    }
    if (!empty()) 
      push_back(Token(",", Token::OTHER));

    new_selector.trim();
    insert(end(), new_selector.begin(), new_selector.end());
    new_selector.clear();
  }

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parsed selector: " << toString();
#endif
}

LessSelector::~LessSelector() {
}

bool LessSelector::parseExtension(Selector &selector, Selector &extension) {
  int parentheses = 1;
  TokenList::iterator i;
  Extension e;

  i = selector.begin();

  // ":", "extend", "("
  if (selector.size() < 3 ||
      (*i).type != Token::COLON ||
      (*++i).type != Token::IDENTIFIER ||
      (*i) != "extend" ||
      (*++i).type != Token::PAREN_OPEN)
    return false;

  i++;
 
  for(; i != selector.end() && parentheses > 0; i++) {
    if ((*i).type == Token::PAREN_OPEN) 
      parentheses++;
    else if ((*i).type == Token::PAREN_CLOSED)
      parentheses--;
    
    if (parentheses > 0) {
      e.getTarget().push_back(*i);
    }
  }
  
  e.setExtension(extension);
  extensions.push_back(e);

  selector.erase(selector.begin(), i);
  
#ifdef WITH_LIBGLOG
  VLOG(2) << "Extension: " << extension.toString();
#endif
  
  return true; 
}

bool LessSelector::parseArguments(TokenList &selector) {
  string delimiter;
  TokenList::iterator i;

  if (selector.front().type != Token::PAREN_OPEN)
    return false;

  if (selector.contains(Token::DELIMITER, ";"))
    delimiter = ";";
  else
    delimiter = ",";

#ifdef WITH_LIBGLOG
  VLOG(3) << "Parameter delimiter: " << delimiter;
#endif
  
  if (!validateArguments(selector, delimiter))
    return false;

  selector.pop_front();

  selector.ltrim();

  while (parseParameter(selector, delimiter)) {
    selector.ltrim();
  }

  i = selector.begin();
  
  if (selector.size() > 3  &&
      (*i) == "." &&
      (*++i) == "." &&
      (*++i) == ".") {
    _unlimitedArguments = true;
    i++;
    selector.erase(selector.begin(), i);
  }

  selector.ltrim();

  if (selector.front().type != Token::PAREN_CLOSED) {
    throw new ParseException(selector.toString(),
                             "matching parentheses.", 0, 0, "");
  }
  selector.pop_front();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Done parsing parameters";
#endif
  
  return true;
}


bool LessSelector::validateArguments(const TokenList &arguments,
                                     const std::string &delimiter) {
  TokenList::const_iterator i = arguments.begin();

  if ((*i).type != Token::PAREN_OPEN)
    return false;

  i++;
  
  while(i != arguments.end() &&
        (*i).type == Token::WHITESPACE) {
    i++;
  }

  while(i != arguments.end()) {
    if ((*i).type == Token::IDENTIFIER) {
      // switch
      i++;
      
    } else if ((*i).type == Token::ATKEYWORD) {
      // variable
      i++;
      
      if ((*i).type == Token::COLON) {
        // default value
        i++;
        while (i != arguments.end() &&
               (*i).type != Token::PAREN_CLOSED &&
               *i != delimiter) {
          i++;
        }
      
      } else if (*i == ".") {
        i++;
        // rest
        if (i == arguments.end() || *i != "." ||
            ++i == arguments.end()  || *i != ".") {
          return false;
        }
        i++;
        break;
      }
    } else 
      break;
        
    if (*i != delimiter)
      break;
    i++;
    
    while(i != arguments.end() && (*i).type == Token::WHITESPACE) {
      i++;
    }
  }
  
  while(i != arguments.end() && (*i).type == Token::WHITESPACE) {
    i++;
  }

  // rest
  if (*i == ".") {
    i++;
    if (i == arguments.end() || *i != "." ||
        ++i == arguments.end() || *i != ".") {
      return false;
    }
    i++;
  }
  if ((*i).type != Token::PAREN_CLOSED) 
    return false;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Validated parameters";
#endif
  
  return true;
}

bool LessSelector::parseParameter(TokenList &selector, const std::string &delimiter) {
  string keyword;
  TokenList value;
  TokenList::iterator i;

  if (selector.empty())
    return false;

  if (selector.front().type == Token::IDENTIFIER) {
    keyword = selector.front();
    selector.pop_front();

  } else if (selector.front().type == Token::ATKEYWORD) {

    keyword = selector.front();
    selector.pop_front();

    i = selector.begin();
    
    if (parseDefaultValue(selector, delimiter, value)) {
      // default value
      
    } else if (selector.size() > 3 &&
               (*i) == "." &&
               (*++i) == "." &&
               (*++i) == ".") {
      i++;
      // rest argument
      selector.erase(selector.begin(), i);
      
      restIdentifier = keyword;
      _unlimitedArguments = true;
      return true;
    }
  } else
    return false;

  selector.ltrim();
  
  if (!selector.empty() &&
      selector.front() == delimiter)
    selector.pop_front();

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parameter: " << keyword << " default: " << value.toString();
#endif
  
  parameters.push_back(keyword);
  defaults.push_back(value);
  return true;
}

bool LessSelector::parseDefaultValue(TokenList &arguments,
                                     const std::string &delimiter,
                                     TokenList &value) {
  unsigned int parentheses = 0;
  
  if (arguments.front().type != Token::COLON)
    return false;
  
  arguments.pop_front();
    
  while (!arguments.empty() &&
         (parentheses > 0 ||
          (arguments.front().type != Token::PAREN_CLOSED &&
           arguments.front() != delimiter))) {
    
    if (arguments.front().type == Token::PAREN_OPEN)
      parentheses++;
    if (arguments.front().type == Token::PAREN_CLOSED)
      parentheses--;

    value.push_back(arguments.front());
    arguments.pop_front();
  }

  value.trim();

  if (value.empty()) {
    throw new ParseException("",
                             "default value following ':'",
                             0, 0, "");
  }
  return true;
}

bool LessSelector::parseConditions (TokenList &selector) {
  TokenList condition;
  
  if (selector.empty() ||
      selector.front() != "when")
    return false;

#ifdef WITH_LIBGLOG
  VLOG(3) << "Parsing conditions";
#endif
  
  selector.pop_front();
  selector.ltrim();
  
  while (!selector.empty()) {
    
    while(!selector.empty() && selector.front() != ",") {
      condition.push_back(selector.front());
      selector.pop_front();
    }
    if (!selector.empty() && selector.front() == ",")
      selector.pop_front();

#ifdef WITH_LIBGLOG
    VLOG(2) << "Condition: " << condition.toString();
#endif
    
    conditions.push_back(condition);
  }
  return true;
}


TokenList* LessSelector::getDefault(const std::string &keyword) {
  std::list<std::string>::iterator pit = parameters.begin();
  std::list<TokenList>::iterator dit = defaults.begin();

  for (;pit != parameters.end(); pit++, dit++) {
    if ((*pit) == keyword)
      return &(*dit);
  }
  return NULL;
}

std::list<std::string>& LessSelector::getParameters() {
  return parameters;
}

std::list<TokenList>& LessSelector::getConditions() {
  return conditions;
}

std::list<Extension>& LessSelector::getExtensions() {
  return extensions;
}

bool LessSelector::matchArguments(const Mixin &mixin) {
  std::list<std::string>::iterator p_it = parameters.begin();
  std::list<TokenList>::iterator d_it = defaults.begin();
  size_t pos = 0;

  for(; p_it != parameters.end(); p_it++, d_it++) {
    
    if (mixin.getArgument(*p_it) == NULL &&
        mixin.getArgument(pos++) == NULL &&
        (*d_it).empty()) {
          return false;
    }
  }
  return (pos >= mixin.getArgumentCount() || unlimitedArguments());
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
