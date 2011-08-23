#include "Parser.h"
#include <iostream>

Parser::Parser(Tokenizer* tokenizer){
  this->tokenizer = tokenizer;
}

bool Parser::parseStylesheet(){
  tokenizer->readNextToken();
  
  skipWhitespace();
  while (parseStatement()) {
    skipWhitespace();
  }

  // return if end of input
  return (tokenizer->getTokenType() == Tokenizer::EOS);
}

void Parser::skipWhitespace () {
  while (tokenizer->getTokenType() == Tokenizer::WHITESPACE ||
         tokenizer->getTokenType() == Tokenizer::COMMENT) {
    tokenizer->readNextToken();
  }
}

bool Parser::parseStatement() {
  return parseRuleset() || parseAtRule();
}

bool Parser::parseAtRule () {
  if (tokenizer->getTokenType() != Tokenizer::ATKEYWORD)
    return false;
  cout << "at rule: " << tokenizer->getToken()->c_str() << endl;
  tokenizer->readNextToken();
  skipWhitespace();
  
  while(parseAny()) {};
  
  if (!parseBlock()) {
    if (tokenizer->getTokenType() != Tokenizer::DELIMITER) {
      throw new ParseException(tokenizer->getToken(),
                               "delimiter (';') at end of @-rule");
    }
    tokenizer->readNextToken();
    skipWhitespace();
  }
  return true;
}

bool Parser::parseBlock () {
  if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN)
    return false;

  cout << "block: " << endl;
  tokenizer->readNextToken();
  skipWhitespace();
  
  while (true) {
    if (!(parseAny() || parseBlock())) {
      if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
        tokenizer->readNextToken();
        skipWhitespace();
      } else if (tokenizer->getTokenType() == Tokenizer::DELIMITER) {
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
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

bool Parser::parseRuleset () {
  if (!parseSelector()) {
    if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN)
      return false;
  } else if (tokenizer->getTokenType() != Tokenizer::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken(),
                             "a declaration block ('{...}')");
  }
  cout << "ruleset " << endl;
  tokenizer->readNextToken();

  skipWhitespace();
  parseDeclaration();
  while (tokenizer->getTokenType() == Tokenizer::DELIMITER) {
    cout << "delimiter " << endl;
    tokenizer->readNextToken();
    skipWhitespace();
    parseDeclaration();
  }
  
  if (tokenizer->getTokenType() != Tokenizer::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of declaration block ('}')");
  } 

  tokenizer->readNextToken();
  skipWhitespace();
  cout << "ruleset end"  << endl;
  return true;
}

vector* Parser::parseSelector() {
  
  if (!parseAny())
    return false;
  cout << "selector " << endl;
  while (parseAny()) {};
  return true;
}

bool Parser::parseDeclaration () {
  string* property = parseProperty();
  if (property == null)
    return false;
  skipWhitespace();
  cout << "declaration " << endl;
  
  if (tokenizer->getTokenType() != Tokenizer::COLON) {
    throw new ParseException(tokenizer->getToken(),
                             "colon following property(':')");
  }
  tokenizer->readNextToken();
  skipWhitespace();
  
  if (!parseValue()) {
    throw new ParseException(tokenizer->getToken(),
                             "value for property");
  }
  cout << "declaration end"  << endl;
  return true;
}

string* Parser::parseProperty () {
  if (tokenizer->getTokenType() != Tokenizer::IDENTIFIER)
    return null;
  string* property = new string(tokenizer->getToken());
  tokenizer->readNextToken();
  return property;
}

bool Parser::parseValue () {
  if (parseAny() || parseBlock()) {
  } else if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
    tokenizer->readNextToken();
    skipWhitespace();
  } else
    return false;

  cout << "value "  << endl;
  while (true) {
    if (parseAny() || parseBlock()) {
    } else if (tokenizer->getTokenType() == Tokenizer::ATKEYWORD) {
      tokenizer->readNextToken();
      skipWhitespace();
    } else 
      return true;
  }
}

bool Parser::parseAny () {
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
    cout << "constant: " <<  tokenizer->getToken()->c_str() << endl;
    tokenizer->readNextToken();
    break;

  case Tokenizer::PAREN_OPEN:
    tokenizer->readNextToken();
    skipWhitespace();
    cout << "paren " << endl;

    while (parseAny() || parseUnused()) {}
    if (tokenizer->getTokenType() != Tokenizer::PAREN_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing parenthesis (')')");
    }
    tokenizer->readNextToken();
    break;
      
  case Tokenizer::IDENTIFIER:
    cout << "identifier: " <<  tokenizer->getToken()->c_str() << endl;
    tokenizer->readNextToken();

    if (tokenizer->getTokenType() == Tokenizer::PAREN_OPEN) {
      tokenizer->readNextToken();
      skipWhitespace();
      while (parseAny() || parseUnused()) {}
      if (tokenizer->getTokenType() != Tokenizer::PAREN_CLOSED) {
        throw new ParseException(tokenizer->getToken(),
                                 "closing parenthesis (')')");
      }
      tokenizer->readNextToken();
    }
    break;
      
  case Tokenizer::BRACE_OPEN:
    tokenizer->readNextToken();
    skipWhitespace();
    cout << "brace" << endl;
    while (parseAny() || parseUnused()) {}
    if (tokenizer->getTokenType() != Tokenizer::BRACE_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing brace (']')");
    }
    tokenizer->readNextToken();
    break;

  default:
    return false;
  }
  skipWhitespace();
  return true;
}

bool Parser::parseUnused() {
  if (parseBlock()) {
  } else if (tokenizer->getTokenType() != Tokenizer::ATKEYWORD) {
    tokenizer->readNextToken();
    skipWhitespace();
  } else if (tokenizer->getTokenType() != Tokenizer::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
  } else
    return false;
  return true;
}
