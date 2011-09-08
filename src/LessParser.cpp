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
  if (value != NULL) 
    valueProcessor->processValue(value);
  
  return value;
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
void LessParser::processParameterRuleset(ParameterRuleset* ruleset) {
  TokenList* selector = ruleset->getSelector();
  TokenListIterator* it = selector->reverseIterator();
  
  while (it->hasPrevious() && it->previous()->type != Token::PAREN_OPEN) {}
  if (!it->hasPrevious()) {
    throw new ParseException(*selector->toString(),
                             "matching parentheses.");
  }
  while (processParameter(it, ruleset)) {
  }
  while (selector->back()->type != Token::PAREN_OPEN) {
    delete selector->pop();
  }
  delete selector->pop();
  parameterRulesets.push_back(ruleset);
  delete it;
}

bool LessParser::processParameter(TokenListIterator* it,
                                  ParameterRuleset* ruleset) {
  Token* current;
  string keyword;
  TokenList* value = NULL;

  while (it->hasNext()) {
    if(it->next()->type != Token::WHITESPACE) {
      it->previous();
      break;
    }
  }

  if (!it->hasNext() || (current = it->next())->type != Token::ATKEYWORD)
    return false;
  
  keyword = current->str;

  if (it->next()->type == Token::COLON) {
    value = new TokenList();
    
    while ((current = it->next())->type != Token::PAREN_CLOSED &&
           current->str != ",") {
      value->push(current->clone());
    }
        
    if (value->size() == 0) {
      throw new ParseException(current->str,
                               "default value following ':'");
    }
  }

  ruleset->addParameter(keyword, value);
  return true;
}

