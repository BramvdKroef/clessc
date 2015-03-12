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

#include <libgen.h>

/**
 * Only allows LessStylesheets
 */
void LessParser::parseStylesheet(LessStylesheet &stylesheet) {
#ifdef WITH_LIBGLOG
  VLOG(1) << "Parser Start";
#endif
  
  CssParser::parseStylesheet(stylesheet);

#ifdef WITH_LIBGLOG
  VLOG(1) << "Parser End";
#endif
}

bool LessParser::parseStatement(Stylesheet &stylesheet) {
  Selector selector;
  Mixin* mixin;
  LessStylesheet* ls = (LessStylesheet*)&stylesheet;
  
  if (parseSelector(selector) && !selector.empty()) {
#ifdef WITH_LIBGLOG
    VLOG(2) << "Parse: Selector: " << selector.toString();
#endif
    
    if (parseRuleset(*ls, selector))
      return true;

    // Parameter mixin in the root. Inserts nested rules but no
    // declarations.
    mixin = ls->createMixin();
    
    if (mixin->parse(selector)) {
      if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokenizer->readNextToken();
        skipWhitespace();
      }
      return true;
    } else {
      ls->deleteMixin(*mixin);
      throw new ParseException(tokenizer->getToken(),
                               "a declaration block ('{...}') following selector",
                               tokenizer->getLineNumber(),
                               tokenizer->getColumn(),
                               tokenizer->getSource());
    }
    
  } else {
    return parseAtRuleOrVariable(*ls);
  }
}

bool LessParser::parseAtRuleOrVariable (LessStylesheet &stylesheet) {
  string keyword;
  Token import;
  TokenList value, rule;
  AtRule* atrule = NULL;
  size_t pathend;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return false;

  keyword = tokenizer->getToken();
  tokenizer->readNextToken();
  skipWhitespace();

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parse: keyword: " << keyword;
#endif
    
  if (parseVariable(value)) {
#ifdef WITH_LIBGLOG
    VLOG(2) << "Parse: variable";
#endif
    stylesheet.putVariable(keyword, value);
    
  } else {
    if (keyword == "@media") {
      parseLessMediaQuery(stylesheet);
      return true;
    }
    
    while(parseAny(rule)) {};
  
    if (!parseBlock(rule)) {
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
    // parse import
    if (keyword == "@import" && rule.size() > 0) {

      if (rule.front().type == Token::URL) {
        import = rule.front().getUrlString();
        
      } else if (rule.front().type == Token::STRING) {
        import = rule.front();
        import.removeQuotes();
        
      } else {
        throw new ParseException(rule.toString(), "A string with the \
file path",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());
      }
        

#ifdef WITH_LIBGLOG
      VLOG(2) << "Import filename: " << import;
#endif
      
      pathend = import.rfind('?');
      if (pathend == std::string::npos)
        pathend = import.size();
      if (pathend < 4 ||
          (import.substr(pathend - 4, 4) != ".css" &&
           import.substr(0, 7) != "http://")) {
        if (pathend < 5 || import.substr(pathend - 5, 5) != ".less") {
          import.insert(pathend, ".less");
          pathend += 5;
        }
        
        importFile(import.substr(0, pathend), stylesheet);
        return true;
      }
    }
    
    atrule = stylesheet.createLessAtRule(keyword);
    atrule->setRule(rule);
  }
  return true;
}

bool LessParser::parseVariable (TokenList &value) {
  if (tokenizer->getTokenType() != Token::COLON)
    return false;
  
  tokenizer->readNextToken();
  skipWhitespace();
    
  if (parseValue(value) == false || value.size() == 0) {
    throw new ParseException(tokenizer->getToken(),
                             "value for variable",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  if (tokenizer->getTokenType() != Token::DELIMITER) {
    throw new ParseException(tokenizer->getToken(),
                             "delimiter (';') at end of @-rule",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();

  return true;
}

bool LessParser::parseSelector(Selector &selector) {
  if (!parseAny(selector)) 
    return false;
    
  while (parseAny(selector) || parseSelectorVariable(selector)) {};
  
  // delete trailing whitespace
  selector.rtrim();
  return true;
}

bool LessParser::parseSelectorVariable(Selector &selector) {
  Token* back;
  
  if (tokenizer->getTokenType() == Token::BRACKET_OPEN) {
    back = &selector.back();
    
    if (back->at(back->length() - 1) == '@') {
      back->append(tokenizer->getToken());
      
      if (tokenizer->readNextToken() != Token::IDENTIFIER) 
        throw new ParseException(tokenizer->getToken(),
                                 "Variable inside selector (e.g.: \
@{identifier})",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());
      back->append(tokenizer->getToken());
      
      if (tokenizer->readNextToken() != Token::BRACKET_CLOSED)
        throw new ParseException(tokenizer->getToken(),
                                 "Closing bracket after variable.",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());

      back->append(tokenizer->getToken());
      tokenizer->readNextToken();
        
      parseWhitespace(selector);

      return true;
    }
  }
  return false;
}

bool LessParser::parseRuleset (LessStylesheet &stylesheet,
                               Selector &selector,
                               LessRuleset* parent) {
  LessRuleset* ruleset;
  
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN)
    return false;

  tokenizer->readNextToken();
  skipWhitespace();

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parse: Ruleset";
#endif

  // Create the ruleset and parse ruleset statements.
  if (parent == NULL) 
    ruleset = stylesheet.createLessRuleset();
  else
    ruleset = parent->createNestedRule();
  
  ruleset->setSelector(selector);
  
  parseRulesetStatements(stylesheet, *ruleset);
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of declaration block ('}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return true;
}

void LessParser::parseRulesetStatements (LessStylesheet &stylesheet,
                                         LessRuleset &ruleset) {
  std::string keyword;
  TokenList value;
  UnprocessedStatement* statement;
  
  while (true) {
    if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      keyword = tokenizer->getToken();
      tokenizer->readNextToken();
      skipWhitespace();
      
      if (parseVariable(value)) {
        ruleset.putVariable(keyword, value);
        value.clear();
        
      } else if (keyword == "@media") {
        parseMediaQueryRuleset(stylesheet, ruleset);
          
      } else {
        throw new ParseException(tokenizer->getToken(),
                                 "Variable declaration after keyword.",
                                 tokenizer->getLineNumber(),
                                 tokenizer->getColumn(),
                                 tokenizer->getSource());
      }

      
    } else if ((statement = parseRulesetStatement(ruleset)) != NULL) {
      // a selector followed by a ruleset is a nested rule
      if (tokenizer->getTokenType() == Token::BRACKET_OPEN) {
        parseRuleset(stylesheet, *statement->getTokens(), &ruleset);
        ruleset.deleteUnprocessedStatement(*statement);
      } 
      
    } else 
      break;
  }
}

void LessParser::parseMediaQueryRuleset(LessStylesheet &stylesheet,
                                        LessRuleset &parent) {

  MediaQueryRuleset* query = parent.createMediaQuery();
  Selector selector;
  Token media("@media", Token::ATKEYWORD);
  Token space(" ", Token::WHITESPACE);
  
  selector.push_back(media);
  selector.push_back(space);

  skipWhitespace();

  while (parseAny(selector) ||
         tokenizer->getTokenType() == Token::ATKEYWORD) {
    if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      selector.push_back(tokenizer->getToken());
      tokenizer->readNextToken();
      parseWhitespace(selector);
    }
  }

  query->setSelector(selector);
  
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken(),
                             "{",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();

  parseRulesetStatements(stylesheet, *query);

  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of media query block ('}')",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
  }
  tokenizer->readNextToken();
  skipWhitespace();
}

UnprocessedStatement* LessParser::parseRulesetStatement (LessRuleset &ruleset) {
  UnprocessedStatement* statement;
  Selector tokens;
  size_t property_i;
  
  parseProperty(tokens);
  property_i = tokens.size();

  parseWhitespace(tokens);
  parseSelector(tokens);
  tokens.trim();

  if (tokens.empty())
    return NULL;

  statement = ruleset.createUnprocessedStatement();
  
  statement->line = tokenizer->getLineNumber();
  statement->column = tokenizer->getColumn();
  statement->source = tokenizer->getSource();
  statement->getTokens()->swap(tokens);
  statement->property_i = property_i;
    
  if (tokenizer->getTokenType() == Token::BRACKET_OPEN) 
    return statement;
  
  parseValue(*statement->getTokens());
  
  if (tokenizer->getTokenType() == Token::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
  } 
  return statement;
}


void LessParser::importFile(const std::string &filename,
                            LessStylesheet &stylesheet) {
  size_t pos = tokenizer->getSource().find_last_of("/\\");

  std::string relative_filename;
  std::list<std::string>::iterator i;
    
  // if the current stylesheet is outside of the current working
  //  directory then add the directory to the filename.
  if (pos != std::string::npos) {
    relative_filename.append(tokenizer->getSource().substr(0, pos + 1));
    relative_filename.append(filename);
  } else
    relative_filename = filename;

  for (i = sources.begin(); i != sources.end(); i++) {
    if ((*i) == relative_filename)
      return;
  }
  
  ifstream in(relative_filename.c_str());
  if (in.fail() || in.bad())
    throw new ParseException(relative_filename, "existing file",
                             tokenizer->getLineNumber(),
                             tokenizer->getColumn(),
                             tokenizer->getSource());
#ifdef WITH_LIBGLOG
  VLOG(1) << "Opening: " << relative_filename;
#endif

  sources.push_back(relative_filename);
  LessTokenizer tokenizer(in, relative_filename);
  LessParser parser(tokenizer, sources);

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parsing";
#endif
  
  parser.parseStylesheet(stylesheet);
  in.close();
}

void LessParser::parseLessMediaQuery(LessStylesheet &stylesheet) { 
  LessMediaQuery* query = stylesheet.createLessMediaQuery();

  Token media("@media", Token::ATKEYWORD);
  Token space(" ", Token::WHITESPACE);
  
  query->getSelector()->push_back(media);
  query->getSelector()->push_back(space);

  skipWhitespace();

  while (parseAny(*query->getSelector()) ||
         tokenizer->getTokenType() == Token::ATKEYWORD) {
    if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      query->getSelector()->push_back(tokenizer->getToken());
      tokenizer->readNextToken();
      parseWhitespace(*query->getSelector());
    }
  }

#ifdef WITH_LIBGLOG
  VLOG(2) << "Media query: " << query->getSelector()->toString();
#endif

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
}
