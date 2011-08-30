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

bool Parser::parseStatement(Stylesheet* stylesheet) {
  Ruleset* ruleset = parseRuleset();
  if (ruleset != null) {
    stylesheet->rulesets.push_back(ruleset);
    return true;
  }
  
  AtRule* atrule = parseAtRule();
  if (atrule != null) {
    stylesheet->atrules.push_back(atrule);
    return true;
  }
  return false;
}

AtRule* Parser::parseAtRule () {
  AtRule* rule = new AtRule();
  if (tokenizer->getTokenType() != Tokenizer::ATKEYWORD)
    return NULL;
  
  rule->keyword = new string(tokenizer->getToken());
  tokenizer->readNextToken();
  skipWhitespace();

  rule->rule = new vector<string*>();
  while(parseAny(rule->rule)) {};
  
  if (!parseBlock(rule->rule)) {
    if (tokenizer->getTokenType() != Tokenizer::DELIMITER) {
      throw new ParseException(tokenizer->getToken(),
                               "delimiter (';') at end of @-rule");
    }
    tokenizer->readNextToken();
    skipWhitespace();
  }
  return true;
}

bool Parser::parseBlock (vector<string*>* tokens) {
  if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN)
    return false;

  tokens->push_back(new string(tokenizer->getToken()));
  tokenizer->readNextToken();
  skipWhitespace();
  
  while (true) {
    if (!(parseAny(tokens) || parseBlock(tokens))) {
      if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
        tokens->push_back(new string(tokenizer->getToken()));
        tokenizer->readNextToken();
        skipWhitespace();
      } else if (tokenizer->getTokenType() == Tokenizer::DELIMITER) {
        tokens->push_back(new string(tokenizer->getToken()));
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
  tokens->push_back(new string(tokenizer->getToken()));
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

Ruleset* Parser::parseRuleset () {
  Ruleset* ruleset = new Ruleset();
  Declaration* declaration = NULL;
  ruleset->selector = parseSelector();
  
  if (selector == NULL) {
    if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN) {
      delete ruleset;
      return NULL;
    }
  } else if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken(),
                             "a declaration block ('{...}')");
  }
  tokenizer->readNextToken();

  skipWhitespace();
  declaration = parseDeclaration();
  if (declaration != null)
    ruleset->declarations.push_back(declaration);
  
  while (tokenizer->getTokenType() == Tokenizer::DELIMITER) {
    cout << "delimiter " << endl;
    tokenizer->readNextToken();
    skipWhitespace();
    declaration = parseDeclaration();
    if (declaration != null)
      ruleset->declarations.push_back(declaration);
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
  cout << "selector " << endl;
  while (parseAny(selector)) {};
  return selector;
}

Declaration* Parser::parseDeclaration () {
  Declaration* declaration = new Declaration();
  declaration->property = parseProperty();
  
  if (declaration->property == NULL) {
    delete declaration;
    return NULL;
  }
  skipWhitespace();
  
  if (tokenizer->getTokenType() != Tokenizer::COLON) {
    throw new ParseException(tokenizer->getToken(),
                             "colon following property(':')");
  }
  tokenizer->readNextToken();
  skipWhitespace();

  declaration->value = parseValue();
  if (value == NULL) {
    throw new ParseException(tokenizer->getToken(),
                             "value for property");
  }
  return declaration;
}

string* Parser::parseProperty () {
  if (tokenizer->getTokenType() != Tokenizer::IDENTIFIER)
    return NULL;
  string* property = new string(tokenizer->getToken());
  tokenizer->readNextToken();
  return property;
}

vector<string*>* Parser::parseValue () {
  vector<string*>* value = new vector<string*>();
  
  if (parseAny(value) || parseBlock(value)) {
  } else if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
    value->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();
  } else {
    delete value;
    return NULL;
  }

  while (true) {
    if (parseAny(value) || parseBlock(value)) {
    } else if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
      value->push_back(new string(tokenizer->getToken()));
      tokenizer->readNextToken();
      skipWhitespace();
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
    tokens->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();
    break;

  case Tokenizer::PAREN_OPEN:
    tokens->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();

    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Tokenizer::PAREN_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing parenthesis (')')");
    }
    tokens->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();
    break;
      
  case Tokenizer::IDENTIFIER:
    tokens->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();

    if (tokenizer->getTokenType() == Tokenizer::PAREN_OPEN) {
      tokens->push_back(new string(tokenizer->getToken()));
      tokenizer->readNextToken();
      skipWhitespace();
      while (parseAny(tokens) || parseUnused(tokens)) {}
      if (tokenizer->getTokenType() != Tokenizer::PAREN_CLOSED) {
        throw new ParseException(tokenizer->getToken(),
                                 "closing parenthesis (')')");
      }
      tokens->push_back(new string(tokenizer->getToken()));
      tokenizer->readNextToken();
    }
    break;
      
  case Tokenizer::BRACE_OPEN:
    tokens->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();
    cout << "brace" << endl;
    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Tokenizer::BRACE_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing brace (']')");
    }
    tokens->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();
    break;

  default:
    return false;
  }
  skipWhitespace();
  return true;
}

bool Parser::parseUnused(vector<string*>* tokens) {
  if (parseBlock(tokens)) {
  } else if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
    tokens->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();
  } else if (tokenizer->getTokenType() == Tokenizer::DELIMITER) {
    tokens->push_back(new string(tokenizer->getToken()));
    tokenizer->readNextToken();
    skipWhitespace();
  } else
    return false;
  return true;
}
