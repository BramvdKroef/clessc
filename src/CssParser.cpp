/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

#include "CssParser.h"
#include <iostream>

CssParser::CssParser(CssTokenizer* tokenizer){
  this->tokenizer = tokenizer;
}

void CssParser::parseStylesheet(Stylesheet* stylesheet){
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
}

void CssParser::skipWhitespace () {
  while (tokenizer->getTokenType() == Token::WHITESPACE ||
         tokenizer->getTokenType() == Token::COMMENT) {
    tokenizer->readNextToken();
  }
}
bool CssParser::parseWhitespace(TokenList* tokens) {
  while (tokenizer->getTokenType() == Token::WHITESPACE ||
         tokenizer->getTokenType() == Token::COMMENT) {
    
    if (tokenizer->getTokenType() == Token::WHITESPACE) {
      tokens->push(new Token(" ", Token::WHITESPACE));
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
  skipWhitespace();
  
  TokenList* rule = new TokenList();
  
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

bool CssParser::parseBlock (TokenList* tokens) {
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN)
    return false;

  tokens->push(tokenizer->getToken()->clone());
  tokenizer->readNextToken();
  skipWhitespace();
  
  while (true) {
    if (!(parseAny(tokens) || parseBlock(tokens))) {
      if (tokenizer->getTokenType() == Token::ATKEYWORD) {
        tokens->push(tokenizer->getToken()->clone());
        tokenizer->readNextToken();
        parseWhitespace(tokens);
      } else if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokens->push(tokenizer->getToken()->clone());
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
  tokens->push(tokenizer->getToken()->clone());
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

Ruleset* CssParser::parseRuleset () {
  Ruleset* ruleset = NULL;
  Declaration* declaration = NULL;
  Selector* selector = new Selector();

  if (!parseSelector(selector)) {
    delete selector;
    selector = NULL;
    
    if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
      return NULL;
    } 
  } else if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    delete selector;
    throw new ParseException(tokenizer->getToken()->str,
                             "a declaration block ('{...}')");
  }
  tokenizer->readNextToken();

  ruleset = new Ruleset(selector);
  
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

bool CssParser::parseSelector(Selector* selector) {
  if (!parseAny(selector)) 
    return false;
    
  while (parseAny(selector)) {};

  // delete trailing whitespace
  while (selector->back()->type == Token::WHITESPACE) {
    delete selector->pop();
  }
  return true;
}

Declaration* CssParser::parseDeclaration () {
  Declaration* declaration = NULL;
  TokenList property;

  if (!parseProperty(&property))
    return NULL;
  
  skipWhitespace();

  declaration = new Declaration(property.toString());
  
  if (tokenizer->getTokenType() != Token::COLON) {
    throw new ParseException(tokenizer->getToken()->str,
                             "colon following property(':')");
  }
  tokenizer->readNextToken();
  skipWhitespace();

  TokenList* value = parseValue();
  if (value == NULL) {
    throw new ParseException(tokenizer->getToken()->str,
                             "value for property");
  }
  declaration->setValue(value);
  return declaration;
}

bool CssParser::parseProperty (TokenList* tokens) {
  if (tokenizer->getToken()->str == "*") {
    // suppor for a IE Hack
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    if (tokenizer->getTokenType() == Token::IDENTIFIER) {
      tokens->push(tokenizer->getToken()->clone());
      tokenizer->readNextToken();
    }
    return true;
    
  } else if (tokenizer->getTokenType() != Token::IDENTIFIER) 
    return false;
  
  tokens->push(tokenizer->getToken()->clone());
  tokenizer->readNextToken();
  return true;
}

TokenList* CssParser::parseValue () {
  TokenList* value = new TokenList();
  
  if (parseAny(value) || parseBlock(value)) {
  } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
    value->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    parseWhitespace(value);
  } else {
    delete value;
    return NULL;
  }

  while (true) {
    if (parseAny(value) || parseBlock(value)) {
    } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      value->push(tokenizer->getToken()->clone());
      tokenizer->readNextToken();
      parseWhitespace(value);
    } else 
      return value;
  }
}

bool CssParser::parseAny (TokenList* tokens) {
  
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
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    break;

  case Token::PAREN_OPEN:
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    skipWhitespace();

    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Token::PAREN_CLOSED) {
      throw new ParseException(tokenizer->getToken()->str,
                               "closing parenthesis (')')");
    }
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    break;
      
  case Token::IDENTIFIER:
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    break;
    
  case Token::BRACE_OPEN:
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    skipWhitespace();
    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Token::BRACE_CLOSED) {
      throw new ParseException(tokenizer->getToken()->str,
                               "closing brace (']')");
    }
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    break;

  default:
    return false;
  }
  parseWhitespace(tokens);
  return true;
}

bool CssParser::parseUnused(TokenList* tokens) {
  if (parseBlock(tokens)) {
  } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    parseWhitespace(tokens);
  } else if (tokenizer->getTokenType() == Token::DELIMITER) {
    tokens->push(tokenizer->getToken()->clone());
    tokenizer->readNextToken();
    skipWhitespace();
  } else
    return false;
  return true;
}
