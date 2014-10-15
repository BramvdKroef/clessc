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

#include "LessParser.h"
#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

/**
 * Only allows LessStylesheets
 */
void LessParser::parseStylesheet(LessStylesheet* stylesheet) {
#ifdef WITH_LIBGLOG
  VLOG(1) << "Parser Start";
#endif
  
  CssParser::parseStylesheet(stylesheet);

#ifdef WITH_LIBGLOG
  VLOG(1) << "Parser End";
#endif
}

bool LessParser::parseStatement(Stylesheet* stylesheet) {
  Selector* selector = new Selector();
  ParameterMixin* mixin;
  
  if (parseSelector(selector) && !selector->empty()) {
#ifdef WITH_LIBGLOG
    VLOG(2) << "Parse: Selector: " << *selector->toString();
#endif
    
    if (parseRuleset((LessStylesheet*)stylesheet, selector))
      return true;

    // Parameter mixin in the root. Inserts nested rules but no
    // declarations.
    mixin = new ParameterMixin();
    
    if (mixin->parse(selector)) {
      ((LessStylesheet*)stylesheet)->addStatement(mixin);
      delete selector;
      if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokenizer->readNextToken();
        skipWhitespace();
      }
      return true;
    } else {
      throw new ParseException(tokenizer->getToken()->str,
                               "a declaration block ('{...}') following selector",
                               tokenizer->getLineNumber(),
                               tokenizer->getColumn(),
                               tokenizer->getSource());
    }
    
  } else {
    delete selector;
    return  parseAtRuleOrVariable((LessStylesheet*)stylesheet);
  }
}

bool LessParser::parseAtRuleOrVariable (LessStylesheet* stylesheet) {
  string keyword, import;
  TokenList value, *rule;
  AtRule* atrule = NULL;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return false;

  keyword = tokenizer->getToken()->str;
  tokenizer->readNextToken();
  skipWhitespace();

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parse: keyword: " << keyword;
#endif
    
  if (parseVariable(&value)) {
#ifdef WITH_LIBGLOG
    VLOG(2) << "Parse: variable";
#endif
    stylesheet->putVariable(keyword, value.clone());
    
  } else {
    if (keyword == "@media") {
      parseLessMediaQuery(stylesheet);
      return true;
    }
    rule = new TokenList();
    while(parseAny(rule)) {};
  
    if (!parseBlock(rule)) {
      if (tokenizer->getTokenType() != Token::DELIMITER) {
        throw new ParseException(tokenizer->getToken()->str,
                                 "delimiter (';') at end of @-rule",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());
      }
      tokenizer->readNextToken();
      skipWhitespace();
    }
    // parse import
    if (keyword == "@import") {
      if (rule->size() != 1 ||
          rule->front()->type != Token::STRING)
        throw new ParseException(*rule->toString(), "A string with the \
file path",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());
      import = rule->front()->str;
#ifdef WITH_LIBGLOG
      VLOG(2) << "Import filename: " << import;
#endif
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
    stylesheet->addStatement(atrule);
  }
  return true;
}

bool LessParser::parseVariable (TokenList* value) {
  if (tokenizer->getTokenType() != Token::COLON)
    return false;
  
  tokenizer->readNextToken();
  skipWhitespace();
    
  if (parseValue(value) == false || value->size() == 0) {
    throw new ParseException(tokenizer->getToken()->str,
                             "value for variable",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  if (tokenizer->getTokenType() != Token::DELIMITER) {
    throw new ParseException(tokenizer->getToken()->str,
                             "delimiter (';') at end of @-rule",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();

  return true;
}

bool LessParser::parseSelector(Selector* selector) {
  if (!parseAny(selector)) 
    return false;
    
  while (parseAny(selector) || parseSelectorVariable(selector)) {};
  
  // delete trailing whitespace
  while (selector->back()->type == Token::WHITESPACE) {
    delete selector->pop();
  }
  return true;
}

bool LessParser::parseSelectorVariable(Selector* selector) {
  string back;
  
  if (tokenizer->getTokenType() == Token::BRACKET_OPEN) {
    back = selector->back()->str;
    
    if (back.at(back.length() - 1) == '@') {
      back.append(tokenizer->getToken()->str);
      
      if (tokenizer->readNextToken() != Token::IDENTIFIER) 
        throw new ParseException(tokenizer->getToken()->str,
                                 "Variable inside selector (e.g.: \
@{identifier})",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());
      back.append(tokenizer->getToken()->str);
      
      if (tokenizer->readNextToken() != Token::BRACKET_CLOSED)
        throw new ParseException(tokenizer->getToken()->str,
                                 "Closing bracket after variable.",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());

      back.append(tokenizer->getToken()->str);
      tokenizer->readNextToken();
      selector->back()->str = back;
        
      parseWhitespace(selector);

      return true;
    }
  }
  return false;
}

bool LessParser::parseRuleset (LessStylesheet* stylesheet,
                               Selector* selector,
                               LessRuleset* parent) {
  LessRuleset* ruleset = NULL;
  
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN)
    return false;

  tokenizer->readNextToken();
  skipWhitespace();

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parse: Ruleset";
#endif

  // Create the ruleset and parse ruleset statements.
  ruleset = new LessRuleset(selector);
  if (parent == NULL) 
    stylesheet->addStatement(ruleset);
  else
    parent->addNestedRule(ruleset);
    
  parseRulesetStatements(stylesheet, ruleset);
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of declaration block ('}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return true;
}

void LessParser::parseRulesetStatements (LessStylesheet* stylesheet,
                                         LessRuleset* ruleset) {
  string keyword;
  TokenList value;
  UnprocessedStatement* statement = new UnprocessedStatement();
  
  while (true) {
    if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      keyword = tokenizer->getToken()->str;
      tokenizer->readNextToken();
      skipWhitespace();
      
      if (parseVariable(&value)) {
        ruleset->putVariable(keyword, value.clone());
        value.clear();
        
      } else if (keyword == "@media") {
        parseMediaQueryRuleset(stylesheet, ruleset);
          
      } else {
        throw new ParseException(tokenizer->getToken()->str,
                                 "Variable declaration after keyword.",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());
      }

      
    } else if (parseRulesetStatement(statement)) {
      // a selector followed by a ruleset is a nested rule
      if (tokenizer->getTokenType() == Token::BRACKET_OPEN) {
        parseRuleset(stylesheet, statement->getTokens()->clone(), ruleset);
        delete statement;
        
      } else
        ruleset->addStatement(statement);
    
      statement = new UnprocessedStatement();
    } else 
      break;
  }
  delete statement;
}

void LessParser::parseMediaQueryRuleset(LessStylesheet* stylesheet,
                                        LessRuleset* parent) {

  MediaQueryRuleset* query = new MediaQueryRuleset();
  Selector* selector = new Selector();
  selector->push(new Token("@media", Token::ATKEYWORD));
  selector->push(new Token(" ", Token::WHITESPACE));

  parseSelector(selector);
  query->setSelector(selector);
  skipWhitespace();
  parent->addNestedRule(query);
  
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken()->str,
                             "{",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();

  parseRulesetStatements(stylesheet, query);

  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of media query block ('}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();
}

bool LessParser::parseRulesetStatement (UnprocessedStatement* statement) {
  statement->line = tokenizer->getLineNumber();
  statement->column = tokenizer->getColumn();
  statement->source = tokenizer->getSource();
  
  parseProperty(statement->getTokens());
  statement->property_i = statement->getTokens()->size();

  parseWhitespace(statement->getTokens());
  parseSelector(statement->getTokens());
  statement->getTokens()->trim();

  if (statement->getTokens()->empty())
    return false;

  if (tokenizer->getTokenType() == Token::BRACKET_OPEN) 
    return true;
  
  parseValue(statement->getTokens());
  
  if (tokenizer->getTokenType() == Token::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
  } 
  return true;
}


void LessParser::importFile(string filename, LessStylesheet* stylesheet) {
  ifstream in(filename.c_str());
  if (in.fail() || in.bad())
    throw new ParseException(filename, "existing file",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
#ifdef WITH_LIBGLOG
  VLOG(1) << "Opening: " << filename;
#endif
  
  LessTokenizer tokenizer(&in, filename);
  LessParser parser(&tokenizer);

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parsing";
#endif
  
  parser.parseStylesheet(stylesheet);
  in.close();
}

void LessParser::parseLessMediaQuery(LessStylesheet* stylesheet) { 
 LessMediaQuery* query = new LessMediaQuery();
  Selector* s = new Selector();
  
  s->push(new Token("@media", Token::ATKEYWORD));
  s->push(new Token(" ", Token::WHITESPACE));

  skipWhitespace();
  parseSelector(s);
  query->setSelector(s);

#ifdef WITH_LIBGLOG
  VLOG(2) << "Media query: " << *s->toString();
#endif

  stylesheet->addStatement(query);

  if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken()->str,
                             "{",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  
  skipWhitespace();
  while (parseStatement(query)) {
    skipWhitespace();
  }
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of media query block ('}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();
}
