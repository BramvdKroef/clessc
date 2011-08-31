#include "Parser.h"
#include <iostream>

Parser::Parser(Tokenizer* tokenizer){
  this->tokenizer = tokenizer;
}

Stylesheet* Parser::parseStylesheet(){
  Stylesheet* stylesheet = new Stylesheet();
  tokenizer->readNextToken();
  
  skipWhitespace();
  while (parseStatement(stylesheet)) {
    skipWhitespace();
  }
  
  // stream should end here
  if (tokenizer->getTokenType() != Tokenizer::EOS) {
    throw new ParseException(tokenizer->getToken(),
                             "end of input");
  }
  return stylesheet;
}

void Parser::skipWhitespace () {
  while (tokenizer->getTokenType() == Tokenizer::WHITESPACE ||
         tokenizer->getTokenType() == Tokenizer::COMMENT) {
    tokenizer->readNextToken();
  }
}
bool Parser::parseWhitespace(vector<string*>* tokens) {
  while (tokenizer->getTokenType() == Tokenizer::WHITESPACE ||
         tokenizer->getTokenType() == Tokenizer::COMMENT) {
    
    if (tokenizer->getTokenType() == Tokenizer::WHITESPACE) {
      tokens->push_back(new string(" "));
    }
    
    tokenizer->readNextToken();
  }
  return true;
}

bool Parser::parseStatement(Stylesheet* stylesheet) {
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

AtRule* Parser::parseAtRule () {
  AtRule* atrule = NULL;
  if (tokenizer->getTokenType() != Tokenizer::ATKEYWORD) 
    return NULL;

  atrule = new AtRule(new string(*tokenizer->getToken()));
  tokenizer->readNextToken();

  vector<string*>* rule = new vector<string*>();
  parseWhitespace(rule);
  
  while(parseAny(rule)) {};
  
  if (!parseBlock(rule)) {
    if (tokenizer->getTokenType() != Tokenizer::DELIMITER) {
      throw new ParseException(tokenizer->getToken(),
                               "delimiter (';') at end of @-rule");
    }
    tokenizer->readNextToken();
    skipWhitespace();
  }
  atrule->setRule(rule);
  return atrule;
}

bool Parser::parseBlock (vector<string*>* tokens) {
  if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN)
    return false;

  tokens->push_back(new string(*tokenizer->getToken()));
  tokenizer->readNextToken();
  skipWhitespace();
  
  while (true) {
    if (!(parseAny(tokens) || parseBlock(tokens))) {
      if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
        tokens->push_back(new string(*tokenizer->getToken()));
        tokenizer->readNextToken();
        parseWhitespace(tokens);
      } else if (tokenizer->getTokenType() == Tokenizer::DELIMITER) {
        tokens->push_back(new string(*tokenizer->getToken()));
        tokenizer->readNextToken();
        skipWhitespace();
      } else
        break;
    }
  }

  if (tokenizer->getTokenType() != Tokenizer::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of block ('}')");
  }
  tokens->push_back(new string(*tokenizer->getToken()));
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

Ruleset* Parser::parseRuleset () {
  Ruleset* ruleset = NULL;
  Declaration* declaration = NULL;
  vector<string*>* selector = parseSelector();
  
  if (selector == NULL) {
    if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN) 
      return NULL;
  } else if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN) {
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
  
  while (tokenizer->getTokenType() == Tokenizer::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
    declaration = parseDeclaration();
    if (declaration != NULL)
      ruleset->addDeclaration(declaration);
  }
  
  if (tokenizer->getTokenType() != Tokenizer::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of declaration block ('}')");
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return ruleset;
}

vector<string*>* Parser::parseSelector() {
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

Declaration* Parser::parseDeclaration () {
  Declaration* declaration = NULL;
  string* property = parseProperty();
  
  if (property == NULL) 
    return NULL;
  
  skipWhitespace();

  declaration = new Declaration(property);
  
  if (tokenizer->getTokenType() != Tokenizer::COLON) {
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

string* Parser::parseProperty () {
  if (tokenizer->getTokenType() != Tokenizer::IDENTIFIER)
    return NULL;
  string* property = new string(*tokenizer->getToken());
  tokenizer->readNextToken();
  return property;
}

vector<string*>* Parser::parseValue () {
  vector<string*>* value = new vector<string*>();
  
  if (parseAny(value) || parseBlock(value)) {
  } else if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
    value->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    parseWhitespace(value);
  } else {
    delete value;
    return NULL;
  }

  while (true) {
    if (parseAny(value) || parseBlock(value)) {
    } else if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
      value->push_back(new string(*tokenizer->getToken()));
      tokenizer->readNextToken();
      parseWhitespace(value);
    } else 
      return value;
  }
}

bool Parser::parseAny (vector<string*>* tokens) {
  
  switch(tokenizer->getTokenType()) {
  case Tokenizer::NUMBER:
  case Tokenizer::PERCENTAGE:
  case Tokenizer::DIMENSION:
  case Tokenizer::STRING:
  case Tokenizer::URL:
  case Tokenizer::HASH:
  case Tokenizer::UNICODE_RANGE:
  case Tokenizer::INCLUDES:
  case Tokenizer::DASHMATCH:
  case Tokenizer::COLON:
  case Tokenizer::OTHER:
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    break;

  case Tokenizer::PAREN_OPEN:
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();

    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Tokenizer::PAREN_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing parenthesis (')')");
    }
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    break;
      
  case Tokenizer::IDENTIFIER:
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();

    if (tokenizer->getTokenType() == Tokenizer::PAREN_OPEN) {
      tokens->push_back(new string(*tokenizer->getToken()));
      tokenizer->readNextToken();
      skipWhitespace();
      while (parseAny(tokens) || parseUnused(tokens)) {}
      if (tokenizer->getTokenType() != Tokenizer::PAREN_CLOSED) {
        throw new ParseException(tokenizer->getToken(),
                                 "closing parenthesis (')')");
      }
      tokens->push_back(new string(*tokenizer->getToken()));
      tokenizer->readNextToken();
    }
    break;
      
  case Tokenizer::BRACE_OPEN:
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();
    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Tokenizer::BRACE_CLOSED) {
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

bool Parser::parseUnused(vector<string*>* tokens) {
  if (parseBlock(tokens)) {
  } else if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    parseWhitespace(tokens);
  } else if (tokenizer->getTokenType() == Tokenizer::DELIMITER) {
    tokens->push_back(new string(*tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();
  } else
    return false;
  return true;
}
