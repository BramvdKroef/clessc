#include "LessParser.h"

bool LessParser::parseStatement(Stylesheet* stylesheet) {
  return parseRuleset(stylesheet) || parseAtRuleOrVariable(stylesheet);
}

bool LessParser::parseAtRuleOrVariable (Stylesheet* stylesheet) {
  string keyword, import;
  TokenList* rule;
  AtRule* atrule = NULL;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return false;

  keyword = tokenizer->getToken()->str;
  tokenizer->readNextToken();
  skipWhitespace();

  if (!parseVariable(keyword)) {
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
    // parse import
    if (keyword == "@import") {
      if (rule->size() != 1 ||
          rule->front()->type != Token::STRING)
        throw new ParseException(*rule->toString(), "A string with the \
file path");
      import = rule->front()->str;
      if (import.size() < 5 ||
          import.substr(import.size() - 5, 4) != ".css") {
        if (import.size() < 6 || import.substr(import.size() - 6, 5) != ".less")
          import.insert(import.size() - 1, ".less");
        
        importFile(import.substr(1, import.size() - 2), stylesheet);
        return true;
      }
    } 
    atrule = new AtRule(new string(keyword));
    atrule->setRule(rule);
    stylesheet->addAtRule(atrule);
  }
  return true;
}

bool LessParser::parseVariable (string keyword = "") {
  TokenList* rule;
  
  if (keyword == "") {
    if (tokenizer->getTokenType() != Token::ATKEYWORD) 
      return false;
    keyword = tokenizer->getToken()->str;
    tokenizer->readNextToken();
    skipWhitespace();

    if (tokenizer->getTokenType() != Token::COLON)
      throw new ParseException(tokenizer->getToken()->str,
                               "colon (':') following @keyword in \
variable declaration.");
  } else if (tokenizer->getTokenType() != Token::COLON)
    return false;
  
  tokenizer->readNextToken();
  skipWhitespace();
    
  rule = parseValue();

  if (rule == NULL || rule->size() == 0) {
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
    
    skipWhitespace();
    parseRulesetStatement(stylesheet, ruleset);

  } else {
    ruleset = new ParameterRuleset(selector);
    valueProcessor->pushScope();
    processParameterRuleset((ParameterRuleset*)ruleset);
    
    skipWhitespace();
    parseRulesetStatement(stylesheet, ruleset);
    
    valueProcessor->popScope();
  }
    
  
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
  Declaration* declaration;
  TokenList* selector = NULL;
  string* property = parseProperty();
    
  // if we can parse a property and the next token is a COLON then the
  // statement is a declaration.
  if (property != NULL) {
    declaration = parseDeclaration(property);
    if (declaration != NULL) {
      ruleset->addDeclaration(declaration);
      if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokenizer->readNextToken();
        skipWhitespace();
        parseRulesetStatement(stylesheet, ruleset);
      }
      return true;
    }
  }
  if (property == NULL && parseVariable()) {
    parseRulesetStatement(stylesheet, ruleset);
    return true;
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
    if (selector->front()->str == "&")
      delete selector->shift();
    else
      selector->unshift(new Token(" ", Token::WHITESPACE));
    selector->unshift(ruleset->getSelector());
    
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

Declaration* LessParser::parseDeclaration (string* property) {
  Declaration* declaration;
  
  skipWhitespace();
  if (tokenizer->getTokenType() != Token::COLON) 
    return NULL;
  tokenizer->readNextToken();
  skipWhitespace();
      
  declaration = new Declaration(property);
      
  TokenList* value = parseValue();
  if (value == NULL) {
    throw new ParseException(tokenizer->getToken()->str,
                             "value for property");
  }
  
  declaration->setValue(value);
  return declaration;
}

TokenList* LessParser::parseValue () {
  TokenList* value = CssParser::parseValue();
  if (value != NULL) 
    valueProcessor->processValue(value);

  return value;
}


void LessParser::parseMixin(TokenList* selector, Ruleset* ruleset,
                            Stylesheet* stylesheet) {
  Ruleset* mixin;
  vector<Declaration*>* declarations;
  vector<Declaration*>::iterator it;
  
  if (processParameterMixin(selector, ruleset)) {

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

bool LessParser::processParameterMixin(TokenList* selector, Ruleset* parent) {
  ParameterRuleset* mixin = getParameterRuleset(selector);
  list<TokenList*>* arguments;
  list<TokenList*>::iterator ait;
  list<string> parameters;
  list<string>::iterator pit;
  vector<Declaration*>* declarations;
  vector<Declaration*>::iterator it;
  Declaration* declaration;
  TokenList* variable;
  TokenList* argsCombined;
  
  if (mixin == NULL)
    return false;
  
  // new scope
  valueProcessor->pushScope();

  // pull values 
  arguments = processArguments(selector);
  parameters = mixin->getKeywords();
  argsCombined = new TokenList();

  // combine with parameter names and add to local scope
  ait = arguments->begin();
  for(pit = parameters.begin(); pit != parameters.end(); pit++) {
    if (ait != arguments->end()) {
      valueProcessor->putVariable(*pit, *ait);
      argsCombined->push((*ait)->clone());
      ait++;
    } else {
      variable = mixin->getDefault(*pit);
      if (variable == NULL) {
        throw new ParseException(*selector->toString(),
                                 "at least one more argument");
      }
      valueProcessor->putVariable(*pit, variable->clone());
      argsCombined->push(variable->clone());
    }
    argsCombined->push(new Token(" ", Token::WHITESPACE));
  }
  delete argsCombined->pop();
  valueProcessor->putVariable("@arguments", argsCombined);
  
  declarations = mixin->getDeclarations();  
  for (it = declarations->begin(); it < declarations->end(); it++) {
    declaration = (*it)->clone();
    valueProcessor->processValue(declaration->getValue());
    parent->addDeclaration(declaration);
  }
    
  valueProcessor->popScope();
  return true;
}

list<TokenList*>* LessParser::processArguments(TokenList* arguments) {
  TokenList* value;
  TokenListIterator* it = arguments->iterator();
  Token* current;
  list<TokenList*>* ret = new list<TokenList*>();
  
  while (it->hasNext() && 
         it->next()->type != Token::PAREN_OPEN) {
  }
  
  while (it->hasNext()) {
    value = new TokenList();
    
    while (it->hasNext()) {
      current = it->next();
      if (current->str == "," || current->type == Token::PAREN_CLOSED)
        break;
      value->push(current->clone());
    }
    valueProcessor->processValue(value);

    ret->push_back(value);
  }
  return ret;
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

void LessParser::processParameterRuleset(ParameterRuleset* ruleset) {
  TokenList* selector = ruleset->getSelector();
  TokenList* newselector = new TokenList();
  list<string> parameters;
  list<string>::iterator pit;
  
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
  ruleset->setSelector(newselector);
  
  while (processParameter(selector, ruleset)) {
  }
  if (selector->front()->type != Token::PAREN_CLOSED) {
    throw new ParseException(*selector->toString(),
                             "matching parentheses.");
  }
  delete selector;
  parameterRulesets.push_back(ruleset);

  // Add a NULL value to the local scope for each parameter so they
  // don't get replaced in the ruleset. For example this statement:
  // @x: 5; .class (@x: 0) {left: @x }
  // 'left: @x' would be replaced with 'left: 5' if we didn't do this
  parameters = ruleset->getKeywords();
  for(pit = parameters.begin(); pit != parameters.end(); pit++) {
    valueProcessor->putVariable(*pit, NULL);
  }
}

bool LessParser::processParameter(TokenList* selector,
                                  ParameterRuleset* ruleset) {
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

  ruleset->addParameter(keyword, value);
  return true;
}

void LessParser::importFile(string filename, Stylesheet* stylesheet) {
  ifstream* in = new ifstream(filename.c_str());
  if (in->fail() || in->bad())
    throw new ParseException(filename, "existing file");
  
  LessTokenizer* tokenizer = new LessTokenizer(in);
  LessParser* parser = new LessParser(tokenizer);
  parser->parseStylesheet(stylesheet);
  in->close();
  delete parser;
  delete tokenizer;
  delete in;
}
