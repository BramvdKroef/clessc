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
  Token* current;
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
           selector->front()->str != ",") {
      value->push(selector->shift());
    }
    
    while (!value->empty() && value->front()->type ==
           Token::WHITESPACE) {
      delete value->shift();
    }
    if (value->empty()) {
      throw new ParseException(current->str,
                               "default value following ':'");
    }
    if (!selector->empty() && selector->front()->str == ",") 
      delete selector->shift();
  }

  addParameter(keyword, value);
  return true;
}
