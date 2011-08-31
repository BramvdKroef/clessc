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
  if (tokenizer->getTokenType() != CssTokenizer::EOS) {
    throw new ParseException(tokenizer->getToken(),
                             "end of input");
  }
  return stylesheet;
}

void CssParser::skipWhitespace () {
  while (tokenizer->getTokenType() == CssTokenizer::WHITESPACE ||
         tokenizer->getTokenType() == CssTokenizer::COMMENT) {
    tokenizer->readNextToken();
  }
}
bool CssParser::parseWhitespace(vector<string*>* tokens) {
  while (tokenizer->getTokenType() == CssTokenizer::WHITESPACE ||
         tokenizer->getTokenType() == CssTokenizer::COMMENT) {
    
    if (tokenizer->getTokenType() == CssTokenizer::WHITESPACE) {
      tokens->push_back(new string(" "));
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
  if (tokenizer->getTokenType() != CssTokenizer::ATKEYWORD) 
    return NULL;

  atrule = new AtRule(new string(*tokenizer->getToken()));
  tokenizer->readNextToken();

  vector<string*>* rule = new vector<string*>();
  parseWhitespace(rule);
  
  while(parseAny(rule)) {};
  
  if (!parseBlock(rule)) {
    if (tokenizer->getTokenType() != CssTokenizer::DELIMITER) {
      throw new ParseException(tokenizer->getToken(),
                               "delimiter (';') at end of @-rule");
    }
    tokenizer->readNextToken();
    skipWhitespace();
  }
  atrule->setRule(rule);
  return atrule;
}

bool CssParser::parseBlock (vector<string*>* tokens) {
  if (tokenizer->getTokenType() != CssTokenizer::BRACKET_OPEN)
    return false;

  tokens->push_back(new string(*tokenizer->getToken()));
  tokenizer->readNextToken();
  skipWhitespace();
  
  while (true) {
    if (!(parseAny(tokens) || parseBlock(tokens))) {
      if (tokenizer->getTokenType() == CssTokenizer::ATKEYWORD) {
        tokens->push_back(new string(*tokenizer->getToken()));
        tokenizer->readNextToken();
        parseWhitespace(tokens);
      } else if (tokenizer->getTokenType() == CssTokenizer::DELIMITER) {
        tokens->push_back(new string(*tokenizer->getToken()));
        tokenizer->readNextToken();
        skipWhitespace();
      } else
        break;
    }
  }

  if (tokenizer->getTokenType() != CssTokenizer::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of block ('}')");
  }
  tokens->push_back(new string(*tokenizer->getToken()));
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

Ruleset* CssParser::parseRuleset () {
  Ruleset* ruleset = NULL;
  Declaration* declaration = NULL;
  vector<string*>* selector = parseSelector();
  
  if (selector == NULL) {
    if (tokenizer->getTokenType() != CssTokenizer::BRACKET_OPEN) 
      return NULL;
  } else if (tokenizer->getTokenType() != CssTokenizer::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken(),
                             "a declaration block ('{...}')");
  }
  tokenizer->readNextToken();

  ruleset = new Ruleset();
  ruleset->setSelector(selector);
  
  skipWhitespace();
  declaration = parseDeclaration();
  if (declaration != NULL)
    ruleset->addDeclaration(declaration);
  
  while (tokenizer->getTokenType() == CssTokenizer::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
    declaration = parseDeclaration();
    if (declaration != NULL)
      ruleset->addDeclaration(declaration);
  }
  
  if (tokenizer->getTokenType() != CssTokenizer::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of declaration block ('}')");
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return ruleset;
}

vector<string*>* CssParser::parseSelector() {
  vector<string*>* selector = new vector<string*>();
  if (!parseAny(selector)) {
    delete selector;
    return NULL;
  }
  
  while (parseAny(selector)) {};

  // delete trailing whitespace
  while (*(selector->back()) == " ") {
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
  
  if (tokenizer->getTokenType() != CssTokenizer::COLON) {
    throw new ParseException(tokenizer->getToken(),
                             "colon following property(':')");
  }
  tokenizer->readNextToken();
  skipWhitespace();

  vector<string*>* value = parseValue();
  if (value == NULL) {
    throw new ParseException(tokenizer->getToken(),
                             "value for property");
  }
  declaration->setValue(value);
  return declaration;
}

string* CssParser::parseProperty () {
  if (tokenizer->getTokenType() != CssTokenizer::IDENTIFIER)
    return NULL;
  string* property = new string(*tokenizer->getToken());
  tokenizer->readNextToken();
  return property;
}

vector<string*>* CssParser::parseValue () {
  vector<string*>* value = new vector<string*>();
  
  if (parseAny(value) || parseBlock(value)) {
  } else if (tokenizer->getTokenType() == CssTokenizer::ATKEYWORD) {
    value->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    parseWhitespace(value);
  } else {
    delete value;
    return NULL;
  }

  while (true) {
    if (parseAny(value) || parseBlock(value)) {
    } else if (tokenizer->getTokenType() == CssTokenizer::ATKEYWORD) {
      value->push_back(new string(*tokenizer->getToken()));
      tokenizer->readNextToken();
      parseWhitespace(value);
    } else 
      return value;
  }
}

bool CssParser::parseAny (vector<string*>* tokens) {
  
  switch(tokenizer->getTokenType()) {
  case CssTokenizer::NUMBER:
  case CssTokenizer::PERCENTAGE:
  case CssTokenizer::DIMENSION:
  case CssTokenizer::STRING:
  case CssTokenizer::URL:
  case CssTokenizer::HASH:
  case CssTokenizer::UNICODE_RANGE:
  case CssTokenizer::INCLUDES:
  case CssTokenizer::DASHMATCH:
  case CssTokenizer::COLON:
  case CssTokenizer::OTHER:
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    break;

  case CssTokenizer::PAREN_OPEN:
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();

    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != CssTokenizer::PAREN_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing parenthesis (')')");
    }
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    break;
      
  case CssTokenizer::IDENTIFIER:
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();

    if (tokenizer->getTokenType() == CssTokenizer::PAREN_OPEN) {
      tokens->push_back(new string(*tokenizer->getToken()));
      tokenizer->readNextToken();
      skipWhitespace();
      while (parseAny(tokens) || parseUnused(tokens)) {}
      if (tokenizer->getTokenType() != CssTokenizer::PAREN_CLOSED) {
        throw new ParseException(tokenizer->getToken(),
                                 "closing parenthesis (')')");
      }
      tokens->push_back(new string(*tokenizer->getToken()));
      tokenizer->readNextToken();
    }
    break;
      
  case CssTokenizer::BRACE_OPEN:
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();
    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != CssTokenizer::BRACE_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing brace (']')");
    }
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    break;

  default:
    return false;
  }
  parseWhitespace(tokens);
  return true;
}

bool CssParser::parseUnused(vector<string*>* tokens) {
  if (parseBlock(tokens)) {
  } else if (tokenizer->getTokenType() == CssTokenizer::ATKEYWORD) {
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    parseWhitespace(tokens);
  } else if (tokenizer->getTokenType() == CssTokenizer::DELIMITER) {
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();
  } else
    return false;
  return true;
}
