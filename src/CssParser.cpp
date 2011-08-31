#include "CssParser.h"
#include <iostream>

CssParser::CssParser(CssTokenizer* tokenizer){
  this->tokenizer = tokenizer;
}

Stylesheet* CssParser::parseStylesheet(){
  Stylesheet* stylesheet = new Stylesheet();
  tokenizer->readNextToken();
  
  skipWhitespace();
  while (parseStatement(stylesheet)) {
    skipWhitespace();
  }
  
  // stream should end here
  if (tokenizer->getTokenType() != Token::EOS) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of input");
  }
  return stylesheet;
}

void CssParser::skipWhitespace () {
  while (tokenizer->getTokenType() == Token::WHITESPACE ||
         tokenizer->getTokenType() == Token::COMMENT) {
    tokenizer->readNextToken();
  }
}
bool CssParser::parseWhitespace(vector<Token*>* tokens) {
  while (tokenizer->getTokenType() == Token::WHITESPACE ||
         tokenizer->getTokenType() == Token::COMMENT) {
    
    if (tokenizer->getTokenType() == Token::WHITESPACE) {
      tokens->push_back(new Token(" ", Token::WHITESPACE));
    }
    
    tokenizer->readNextToken();
  }
  return true;
}

bool CssParser::parseStatement(Stylesheet* stylesheet) {
  Ruleset* ruleset = parseRuleset();
  if (ruleset != NULL) {
    stylesheet->addRuleset(ruleset);
    return true;
  }
  
  AtRule* atrule = parseAtRule();
  if (atrule != NULL) {
    stylesheet->addAtRule(atrule);
    return true;
  }
  return false;
}

AtRule* CssParser::parseAtRule () {
  AtRule* atrule = NULL;
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return NULL;

  atrule = new AtRule(new string(tokenizer->getToken()->str));
  tokenizer->readNextToken();

  vector<Token*>* rule = new vector<Token*>();
  parseWhitespace(rule);
  
  while(parseAny(rule)) {};
  
  if (!parseBlock(rule)) {
    if (tokenizer->getTokenType() != Token::DELIMITER) {
      throw new ParseException(tokenizer->getToken()->str,
                               "delimiter (';') at end of @-rule");
    }
    tokenizer->readNextToken();
    skipWhitespace();
  }
  atrule->setRule(rule);
  return atrule;
}

bool CssParser::parseBlock (vector<Token*>* tokens) {
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN)
    return false;

  tokens->push_back(tokenizer->getToken()->clone());
  tokenizer->readNextToken();
  skipWhitespace();
  
  while (true) {
    if (!(parseAny(tokens) || parseBlock(tokens))) {
      if (tokenizer->getTokenType() == Token::ATKEYWORD) {
        tokens->push_back(tokenizer->getToken()->clone());
        tokenizer->readNextToken();
        parseWhitespace(tokens);
      } else if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokens->push_back(tokenizer->getToken()->clone());
        tokenizer->readNextToken();
        skipWhitespace();
      } else
        break;
    }
  }

  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of block ('}')");
  }
  tokens->push_back(tokenizer->getToken()->clone());
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

Ruleset* CssParser::parseRuleset () {
  Ruleset* ruleset = NULL;
  Declaration* declaration = NULL;
  vector<Token*>* selector = parseSelector();
  
  if (selector == NULL) {
    if (tokenizer->getTokenType() != Token::BRACKET_OPEN) 
      return NULL;
  } else if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken()->str,
                             "a declaration block ('{...}')");
  }
  tokenizer->readNextToken();

  ruleset = new Ruleset();
  ruleset->setSelector(selector);
  
  skipWhitespace();
  declaration = parseDeclaration();
  if (declaration != NULL)
    ruleset->addDeclaration(declaration);
  
  while (tokenizer->getTokenType() == Token::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
    declaration = parseDeclaration();
    if (declaration != NULL)
      ruleset->addDeclaration(declaration);
  }
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of declaration block ('}')");
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return ruleset;
}

vector<Token*>* CssParser::parseSelector() {
  vector<Token*>* selector = new vector<Token*>();
  if (!parseAny(selector)) {
    delete selector;
    return NULL;
  }
  
  while (parseAny(selector)) {};

  // delete trailing whitespace
  while (selector->back()->type == Token::WHITESPACE) {
    delete selector->back();
    selector->pop_back();
  }
  return selector;
}

Declaration* CssParser::parseDeclaration () {
  Declaration* declaration = NULL;
  string* property = parseProperty();
  
  if (property == NULL) 
    return NULL;
  
  skipWhitespace();

  declaration = new Declaration(property);
  
  if (tokenizer->getTokenType() != Token::COLON) {
    throw new ParseException(tokenizer->getToken()->str,
                             "colon following property(':')");
  }
  tokenizer->readNextToken();
  skipWhitespace();

  vector<Token*>* value = parseValue();
  if (value == NULL) {
    throw new ParseException(tokenizer->getToken()->str,
                             "value for property");
  }
  declaration->setValue(value);
  return declaration;
}

string* CssParser::parseProperty () {
  if (tokenizer->getTokenType() != Token::IDENTIFIER)
    return NULL;
  string* property = new string(tokenizer->getToken()->str);
  tokenizer->readNextToken();
  return property;
}

vector<Token*>* CssParser::parseValue () {
  vector<Token*>* value = new vector<Token*>();
  
  if (parseAny(value) || parseBlock(value)) {
  } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
    value->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    parseWhitespace(value);
  } else {
    delete value;
    return NULL;
  }

  while (true) {
    if (parseAny(value) || parseBlock(value)) {
    } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      value->push_back(tokenizer->getToken()->clone());
      tokenizer->readNextToken();
      parseWhitespace(value);
    } else 
      return value;
  }
}

bool CssParser::parseAny (vector<Token*>* tokens) {
  
  switch(tokenizer->getTokenType()) {
  case Token::NUMBER:
  case Token::PERCENTAGE:
  case Token::DIMENSION:
  case Token::STRING:
  case Token::URL:
  case Token::HASH:
  case Token::UNICODE_RANGE:
  case Token::INCLUDES:
  case Token::DASHMATCH:
  case Token::COLON:
  case Token::OTHER:
    tokens->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    break;

  case Token::PAREN_OPEN:
    tokens->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    skipWhitespace();

    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Token::PAREN_CLOSED) {
      throw new ParseException(tokenizer->getToken()->str,
                               "closing parenthesis (')')");
    }
    tokens->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    break;
      
  case Token::IDENTIFIER:
    tokens->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();

    if (tokenizer->getTokenType() == Token::PAREN_OPEN) {
      tokens->push_back(tokenizer->getToken()->clone());
      tokenizer->readNextToken();
      skipWhitespace();
      while (parseAny(tokens) || parseUnused(tokens)) {}
      if (tokenizer->getTokenType() != Token::PAREN_CLOSED) {
        throw new ParseException(tokenizer->getToken()->str,
                                 "closing parenthesis (')')");
      }
      tokens->push_back(tokenizer->getToken()->clone());
      tokenizer->readNextToken();
    }
    break;
      
  case Token::BRACE_OPEN:
    tokens->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    skipWhitespace();
    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Token::BRACE_CLOSED) {
      throw new ParseException(tokenizer->getToken()->str,
                               "closing brace (']')");
    }
    tokens->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    break;

  default:
    return false;
  }
  parseWhitespace(tokens);
  return true;
}

bool CssParser::parseUnused(vector<Token*>* tokens) {
  if (parseBlock(tokens)) {
  } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
    tokens->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    parseWhitespace(tokens);
  } else if (tokenizer->getTokenType() == Token::DELIMITER) {
    tokens->push_back(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    skipWhitespace();
  } else
    return false;
  return true;
}
