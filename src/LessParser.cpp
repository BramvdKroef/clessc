#include "LessParser.h"

bool LessParser::parseStatement(Stylesheet* stylesheet) {
  return parseRuleset(stylesheet) || parseAtRuleOrVariable(stylesheet);
}

bool LessParser::parseAtRuleOrVariable (Stylesheet* stylesheet) {
  string keyword;
  TokenList* rule;
  AtRule* atrule = NULL;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return false;

  keyword = tokenizer->getToken()->str;
  tokenizer->readNextToken();
  skipWhitespace();

  if (tokenizer->getTokenType() == Token::COLON) {
    tokenizer->readNextToken();
    skipWhitespace();
    
    rule = parseValue();
    if (rule == NULL) {
      throw new ParseException(tokenizer->getToken()->str,
                               "value for variable");
    }
    if (tokenizer->getTokenType() != Token::DELIMITER) {
      throw new ParseException(tokenizer->getToken()->str,
                               "delimiter (';') at end of @-rule");
    }
    valueProcessor->putVariable(keyword, rule);
    tokenizer->readNextToken();
    skipWhitespace();
    
  } else {
    rule = new TokenList();
    while(parseAny(rule)) {};
  
    if (!parseBlock(rule)) {
      if (tokenizer->getTokenType() != Token::DELIMITER) {
        throw new ParseException(tokenizer->getToken()->str,
                                 "delimiter (';') at end of @-rule");
      }
      tokenizer->readNextToken();
      skipWhitespace();
    }
    atrule = new AtRule(new string(keyword));
    atrule->setRule(rule);
    stylesheet->addAtRule(atrule);
  }
  return true;
}

bool LessParser::parseRuleset (Stylesheet* stylesheet,
                                   TokenList* selector) {
  Ruleset* ruleset = NULL;
  if (selector == NULL)
    selector = parseSelector();
  
  if (selector == NULL) {
    if (tokenizer->getTokenType() != Token::BRACKET_OPEN) 
      return false;
  } else if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken()->str,
                             "a declaration block ('{...}')");
  }
  tokenizer->readNextToken();

  if (selector->back()->type != Token::PAREN_CLOSED) {
    ruleset = new Ruleset(selector);
    stylesheet->addRuleset(ruleset);
  } else {
    ruleset = new ParameterRuleset(selector);
    processParameterRuleset((ParameterRuleset*)ruleset);
  }
    
  skipWhitespace();
  parseRulesetStatement(stylesheet, ruleset);
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of declaration block ('}')");
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return true;
}

bool LessParser::parseRulesetStatement (Stylesheet* stylesheet,
                                        Ruleset* ruleset) {
  Declaration* declaration = NULL;
  TokenList* selector = NULL;
  string* property = parseProperty();
    
  // if we can parse a property and the next token is a COLON then the
  // statement is a declaration.
  if (property != NULL) {
    skipWhitespace();
    if (tokenizer->getTokenType() == Token::COLON) {
      tokenizer->readNextToken();
      skipWhitespace();
      
      declaration = new Declaration(property);
      
      TokenList* value = parseValue();
      if (value == NULL) {
        throw new ParseException(tokenizer->getToken()->str,
                                 "value for property");
      }
      declaration->setValue(value);
      ruleset->addDeclaration(declaration);

      if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokenizer->readNextToken();
        skipWhitespace();
        parseRulesetStatement(stylesheet, ruleset);
      }
      return true;
    }
  }
  
  // otherwise parse a selector
  selector = parseSelector();
  if (selector == NULL && property == NULL)
    return false;

  // Insert property at the front
  if (selector == NULL)
    selector = new TokenList();
  if (property != NULL) {
    selector->unshift(new Token(*property, Token::IDENTIFIER));
    delete property;
  }

  // if followed by a ruleset it's a nested rule
  if (tokenizer->getTokenType() == Token::BRACKET_OPEN) {
    processNestedSelector(ruleset->getSelector(), selector);
    parseRuleset(stylesheet, selector);
    
    parseRulesetStatement(stylesheet, ruleset);
  } else {
    // otherwise it's a mixin.
    parseMixin(selector, ruleset, stylesheet);
    
    if (tokenizer->getTokenType() == Token::DELIMITER) {
      tokenizer->readNextToken();
      skipWhitespace();
      parseRulesetStatement(stylesheet, ruleset);
    }
  }
  return true;
}

TokenList* LessParser::parseValue () {
  TokenList* value = CssParser::parseValue();
  if (value != NULL) 
    valueProcessor->processValue(value);
  cout << "processed: " << *value->toString() << endl;
  return value;
}

void LessParser::parseMixin(TokenList* selector, Ruleset* ruleset,
                            Stylesheet* stylesheet) {
  ParameterRuleset* pmixin = getParameterRuleset(selector);
  Ruleset* mixin;
  vector<Declaration*>* declarations;
  vector<Declaration*>::iterator it;
  
  if (pmixin != NULL) {
    declarations = pmixin->getDeclarations();
    for (it = declarations->begin(); it < declarations->end(); it++) {
      ruleset->addDeclaration((*it)->clone());
    }

  } else if((mixin = stylesheet->getRuleset(selector)) != NULL) {
    declarations = mixin->getDeclarations();
    for (it = declarations->begin(); it < declarations->end(); it++) {
      ruleset->addDeclaration((*it)->clone());
    }
  } else {
    throw new ParseException(*selector->toString(),
                             "a mixin that has been defined");
  }
}

ParameterRuleset* LessParser::getParameterRuleset(TokenList* selector) {
  vector<ParameterRuleset*>::iterator it;
  TokenList key;
  TokenListIterator* tli = selector->iterator();
  Token* current;

  while (tli->hasNext()) {
    current = tli->next();
    if (current->type == Token::PAREN_OPEN)
      break;
    key.push(current->clone());
  }
  delete tli;
  
  for (it = parameterRulesets.begin(); it < parameterRulesets.end(); it++) {
    if ((*it)->getSelector()->equals(&key)) 
      return *it;
  }
  return NULL;
}

void LessParser::processNestedSelector(TokenList* parent, TokenList* nested) {
  if (nested->front()->str == "&")
    delete nested->shift();
  else
    nested->unshift(new Token(" ", Token::WHITESPACE));
  nested->unshift(parent);
}
void LessParser::processParameterRuleset(ParameterRuleset* ruleset) {
  TokenList* selector = ruleset->getSelector();
  TokenList* newselector = new TokenList();

  while (selector->size() > 0 &&
         selector->front()->type != Token::PAREN_OPEN) {
    newselector->push(selector->shift());
  }
  if (selector->size() == 0) {
    throw new ParseException(*selector->toString(),
                             "matching parentheses.");
  }
  
  delete selector->shift();
  while (newselector->back()->type == Token::WHITESPACE) 
    delete newselector->pop();
  ruleset->setSelector(newselector);
  
  while (processParameter(selector, ruleset)) {
  }
  if (selector->front()->type != Token::PAREN_CLOSED) {
    throw new ParseException(*selector->toString(),
                             "matching parentheses.");
  }
  delete selector;
  parameterRulesets.push_back(ruleset);
}

bool LessParser::processParameter(TokenList* selector,
                                  ParameterRuleset* ruleset) {
  Token* current;
  string keyword;
  TokenList* value = NULL;

  while (selector->size() > 0 &&
         selector->front()->type == Token::WHITESPACE) {
    delete selector->shift();
  }

  if (selector->size() == 0 ||
      selector->front()->type != Token::ATKEYWORD)
    return false;

  keyword = selector->front()->str;
  delete selector->shift();
  
  if (selector->front()->type == Token::COLON) {
    delete selector->shift();
    value = new TokenList();
    
    while (selector->size() > 0 &&
           selector->front()->type != Token::PAREN_CLOSED &&
           selector->front()->str != ",") {
      value->push(selector->shift());
    }
    
    if (value->size() == 0) {
      throw new ParseException(current->str,
                               "default value following ':'");
    }
    if (selector->size() > 0 && selector->front()->str == ",") 
      delete selector->shift();
  }

  ruleset->addParameter(keyword, value);
  return true;
}

