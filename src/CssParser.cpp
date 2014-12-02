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

CssParser::CssParser(CssTokenizer &tokenizer){
  this->tokenizer = &tokenizer;
}

void CssParser::parseStylesheet(Stylesheet &stylesheet){
  tokenizer->readNextToken();
  
  skipWhitespace();
  while (parseStatement(stylesheet)) {
    skipWhitespace();
  }
  
  // stream should end here
  if (tokenizer->getTokenType() != Token::EOS) {
    throw new ParseException(tokenizer->getToken(),
                             "end of input",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
}

void CssParser::skipWhitespace () {
  while (tokenizer->getTokenType() == Token::WHITESPACE ||
         tokenizer->getTokenType() == Token::COMMENT) {
    tokenizer->readNextToken();
  }
}
bool CssParser::parseWhitespace(TokenList &tokens) {
  Token space(" ", Token::WHITESPACE);
  
  while (tokenizer->getTokenType() == Token::WHITESPACE ||
         tokenizer->getTokenType() == Token::COMMENT) {
    
    if (tokenizer->getTokenType() == Token::WHITESPACE) {
      tokens.push_back(space);
    }
    
    tokenizer->readNextToken();
  }
  return true;
}

bool CssParser::parseStatement(Stylesheet &stylesheet) {
  Ruleset* ruleset = parseRuleset(stylesheet);
  if (ruleset != NULL)
    return true;
  
  MediaQuery* query = parseMediaQuery(stylesheet);
  if (query != NULL) 
    return true;
  
  AtRule* atrule = parseAtRule(stylesheet);
  if (atrule != NULL) 
    return true;
  
  return false;
}

MediaQuery* CssParser::parseMediaQuery(Stylesheet &stylesheet) {
  MediaQuery* query;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD ||
      tokenizer->getToken() != "@media") 
    return NULL;

  query = stylesheet.createMediaQuery();

  query->getSelector().push_back(tokenizer->getToken());

  tokenizer->readNextToken();
  skipWhitespace();
  
  parseSelector(query->getSelector());
  
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken(),
                             "{",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  
  skipWhitespace();
  while (parseStatement(*query)) {
    skipWhitespace();
  }
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of media query block ('}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();
  return query;
}

AtRule* CssParser::parseAtRule(Stylesheet& stylesheet) {
  AtRule* atrule;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return NULL;

  atrule = stylesheet.createAtRule(tokenizer->getToken());
  tokenizer->readNextToken();
  skipWhitespace();
  
  while(parseAny(atrule->getRule())) {};
  
  if (!parseBlock(atrule->getRule())) {
    if (tokenizer->getTokenType() != Token::DELIMITER) {
      throw new ParseException(tokenizer->getToken(),
                               "delimiter (';') at end of @-rule",
                               tokenizer->getLineNumber(),
                               tokenizer->getColumn(),
                               tokenizer->getSource());
    }
    tokenizer->readNextToken();
    skipWhitespace();
  }
  
  return atrule;
}

bool CssParser::parseBlock (TokenList &tokens) {
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN)
    return false;

  tokens.push_back(tokenizer->getToken());
  tokenizer->readNextToken();
  skipWhitespace();
  
  while (true) {
    if (!(parseAny(tokens) || parseBlock(tokens))) {
      
      if (tokenizer->getTokenType() == Token::ATKEYWORD) {
        tokens.push_back(tokenizer->getToken());
        tokenizer->readNextToken();
        parseWhitespace(tokens);
        
      } else if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokens.push_back(tokenizer->getToken());
        tokenizer->readNextToken();
        skipWhitespace();
        
      } else
        break;
    }
  }

  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of block ('}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokens.push_back(tokenizer->getToken());
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

Ruleset* CssParser::parseRuleset (Stylesheet &stylesheet) {
  Ruleset* ruleset;
  Selector selector;

  if (!parseSelector(selector)) {

    if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
      return NULL;
    } 
  } else if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken(),
                             "a declaration block ('{...}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();

  ruleset = stylesheet.createRuleset(selector);
  
  skipWhitespace();
  parseDeclaration(*ruleset);
    
  while (tokenizer->getTokenType() == Token::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
    parseDeclaration(*ruleset);
  }
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of declaration block ('}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return ruleset;
}

bool CssParser::parseSelector(Selector &selector) {
  if (!parseAny(selector)) 
    return false;
    
  while (parseAny(selector)) {};

  // delete trailing whitespace
  selector.rtrim();
  return true;
}

Declaration* CssParser::parseDeclaration (Ruleset &ruleset) {
  Declaration* declaration = NULL;
  TokenList property;

  if (!parseProperty(property))
    return NULL;
  
  skipWhitespace();

  declaration = ruleset.createDeclaration(property.toString());
  
  if (tokenizer->getTokenType() != Token::COLON) {
    throw new ParseException(tokenizer->getToken(),
                             "colon following property(':')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();

  if (!parseValue(declaration->getValue())) {
    throw new ParseException(tokenizer->getToken(),
                             "value for property",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  return declaration;
}

bool CssParser::parseProperty (TokenList &tokens) {
  if (tokenizer->getToken() == "*") {
    // suppor for an IE Hack
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    
    if (tokenizer->getTokenType() == Token::IDENTIFIER) {
      tokens.push_back(tokenizer->getToken());
      tokenizer->readNextToken();
    }
    return true;
    
  } else if (tokenizer->getTokenType() != Token::IDENTIFIER) 
    return false;
  
  tokens.push_back(tokenizer->getToken());
  tokenizer->readNextToken();
  return true;
}

bool CssParser::parseValue (TokenList &value) {
  
  if (parseAny(value) || parseBlock(value)) {
    
  } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
    value.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    parseWhitespace(value);
    
  } else {
    return false;
  }

  while (true) {
    if (parseAny(value) || parseBlock(value)) {
      
    } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      value.push_back(tokenizer->getToken());
      tokenizer->readNextToken();
      parseWhitespace(value);
      
    } else 
      return true;
  }
}

bool CssParser::parseAny (TokenList &tokens) {
  
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
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    break;

  case Token::PAREN_OPEN:
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    skipWhitespace();

    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Token::PAREN_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing parenthesis (')')",
                               tokenizer->getLineNumber(),
                               tokenizer->getColumn(),
                               tokenizer->getSource());
    }
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    break;
      
  case Token::IDENTIFIER:
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    break;
    
  case Token::BRACE_OPEN:
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    skipWhitespace();
    while (parseAny(tokens) || parseUnused(tokens)) {}
    if (tokenizer->getTokenType() != Token::BRACE_CLOSED) {
      throw new ParseException(tokenizer->getToken(),
                               "closing brace (']')",
                               tokenizer->getLineNumber(),
                               tokenizer->getColumn(),
                               tokenizer->getSource());
    }
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    break;

  default:
    return false;
  }
  parseWhitespace(tokens);
  return true;
}

bool CssParser::parseUnused(TokenList& tokens) {
  if (parseBlock(tokens)) {
  } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    parseWhitespace(tokens);
  } else if (tokenizer->getTokenType() == Token::DELIMITER) {
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    skipWhitespace();
  } else
    return false;
  return true;
}
