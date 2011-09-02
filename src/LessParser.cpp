#include "LessParser.h"

bool LessParser::parseStatement(Stylesheet* stylesheet) {
  return parseRuleset(stylesheet) || parseAtRuleOrVariable(stylesheet);
}

bool LessParser::parseAtRuleOrVariable (Stylesheet* stylesheet) {
  string* keyword = NULL;
  TokenList* rule;
  AtRule* atrule = NULL;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return false;

  keyword = new string(tokenizer->getToken()->str);
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
    variables[*(keyword)] = rule;
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
    atrule = new AtRule(keyword);
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

  ruleset = new Ruleset();
  ruleset->setSelector(selector);
  if (selector->back()->type != Token::PAREN_CLOSED)
    stylesheet->addRuleset(ruleset);
    
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

  // if followed by a ruleset it's a nested rule, otherwise it's a
  // mixin.
  if (tokenizer->getTokenType() == Token::BRACKET_OPEN) {
    processNestedSelector(ruleset->getSelector(), selector);
    parseRuleset(stylesheet, selector);
    
    parseRulesetStatement(stylesheet, ruleset);
  } else {
    Ruleset* mixin = stylesheet->getRuleset(selector);
    if (mixin == NULL)
      throw new ParseException(*selector->toString(),
                               "a mixin that has been defined");

    processMixin(ruleset, mixin);
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
  if (value != NULL) {
    TokenList* newvalue = processValue(value);
    delete value;
    return newvalue;
  } else
    return value;
}

TokenList* LessParser::processValue(TokenList* value) {
  TokenList* newvalue = new TokenList();
  TokenListIterator* it = value->iterator();
  Token* token = NULL;
  
  while(it->hasNext()) {
    token = it->next();
    
    if (processVariable(token, newvalue)) {
    } else if (it->hasNext(),
               processDeepVariable(token, it->peek(), newvalue)) { 
      it->next();
    } else
      newvalue->push(token->clone());
  }
  return newvalue;
}

bool LessParser::processVariable (Token* token, TokenList* newvalue) {
  if (token->type == Token::ATKEYWORD && variables.count(token->str)) {
    newvalue->push(variables[token->str]);
    return true;
  } else
    return false;
}

bool LessParser::processDeepVariable (Token* token, Token* nexttoken,
                                      TokenList* newvalue) {
  TokenList* var;
  string key("@");
  
  if (token->type != Token::OTHER ||
      token->str != "@")
    return false;
  
  if (nexttoken->type != Token::ATKEYWORD ||
      !variables.count(nexttoken->str))
    return false;
  
  var = variables[nexttoken->str];

  if (var->size() > 1 ||
      var->front()->type != Token::STRING)
    return false;

  // generate key with '@' + var without quotes
  key.append(var->front()->
             str.substr(1, var->front()->str.size() - 2));

  if (!variables.count(key))
    return false;
  
  var = variables[key];
  newvalue->push(var);
  return true;
}

void LessParser::processMixin(Ruleset* parent, Ruleset* mixin) {
  vector<Declaration*>* declarations = mixin->getDeclarations();
  vector<Declaration*>::iterator it;

  for (it = declarations->begin(); it < declarations->end(); it++) {
    parent->addDeclaration((*it)->clone());
  }
}

void LessParser::processNestedSelector(TokenList* parent, TokenList* nested) {
  if (nested->front()->str == "&")
    delete nested->shift();
  else
    nested->unshift(new Token(" ", Token::WHITESPACE));
  nested->unshift(parent);
}
