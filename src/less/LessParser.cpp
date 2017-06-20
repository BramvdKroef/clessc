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

void LessParser::skipWhitespace () {
  while (tokenizer->getTokenType() == Token::WHITESPACE ||
         (tokenizer->getTokenType() == Token::COMMENT && 
          tokenizer->getToken().compare(0, 2, "//") == 0)) {
    tokenizer->readNextToken();
  }
}

bool LessParser::parseStatement(Stylesheet &stylesheet) {
  Selector selector;
  Mixin* mixin;
  CssComment* comment;
  LessStylesheet* ls = (LessStylesheet*)&stylesheet;

  if (tokenizer->getTokenType() == Token::COMMENT) {
    comment = ls->createComment();
    comment->setComment(tokenizer->getToken());
    tokenizer->readNextToken();
    skipWhitespace();
    return true;
    
  } else if (parseSelector(selector) && !selector.empty()) {
#ifdef WITH_LIBGLOG
    VLOG(2) << "Parse: Selector: " << selector.toString();
#endif
    
    if (parseRuleset(*ls, selector))
      return true;

    // Parameter mixin in the root. Inserts nested rules but no
    // declarations.
    mixin = ls->createMixin();
    mixin->setReference(reference);
    
    if (mixin->parse(selector)) {
      if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokenizer->readNextToken();
        skipWhitespace();
      }
      return true;
    } else {
      ls->deleteMixin(*mixin);
      throw new ParseException(tokenizer->getToken(),
                               "a declaration block ('{...}') following selector");
    }
    
  } else {
    return parseAtRuleOrVariable(*ls);
  }
}

bool LessParser::parseAtRuleOrVariable (LessStylesheet &stylesheet) {
  Token token;
  TokenList value, rule;
  AtRule* atrule = NULL;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return false;

  token = tokenizer->getToken();
  tokenizer->readNextToken();
  CssParser::skipWhitespace();

#ifdef WITH_LIBGLOG
  VLOG(2) << "Parse: keyword: " << token;
#endif
    
  if (parseVariable(value)) {
#ifdef WITH_LIBGLOG
    VLOG(2) << "Parse: variable";
#endif
    stylesheet.putVariable(token, value);
    
  } else {
    if (token == "@media") {
      parseLessMediaQuery(token, stylesheet);
      return true;
    }

    parseAtRuleValue(rule);
    
    // parse import
    if (token == "@import" && rule.size() > 0) {
      if (parseImportStatement(rule, stylesheet))
        return true;
    }
    
    atrule = stylesheet.createLessAtRule(token);
    atrule->setReference(reference);
    atrule->setRule(rule);
  }
  return true;
}

bool LessParser::parseAtRuleValue(TokenList &rule) {
  while(parseAny(rule)) {};
  
  if (!parseBlock(rule)) {
    if (tokenizer->getTokenType() != Token::DELIMITER) {
      throw new ParseException(tokenizer->getToken(),
                               "delimiter (';') at end of @-rule");
    }
    tokenizer->readNextToken();
    skipWhitespace();
  }
  return true;
}

bool LessParser::parseVariable (TokenList &value) {
  if (tokenizer->getTokenType() != Token::COLON)
    return false;
  
  tokenizer->readNextToken();
  CssParser::skipWhitespace();
    
  if (parseValue(value) == false || value.size() == 0) {
    throw new ParseException(tokenizer->getToken(),
                             "value for variable");
  }
  if (tokenizer->getTokenType() != Token::DELIMITER) {
    throw new ParseException(tokenizer->getToken(),
                             "delimiter (';') at end of @-rule");
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
@{identifier})");
      back->append(tokenizer->getToken());
      
      if (tokenizer->readNextToken() != Token::BRACKET_CLOSED)
        throw new ParseException(tokenizer->getToken(),
                                 "Closing bracket after variable.");

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
  ruleset->setReference(reference);
  ruleset->setSelector(selector);
  
  parseRulesetStatements(stylesheet, *ruleset);
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of declaration block ('}')");
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return true;
}

void LessParser::parseRulesetStatements (LessStylesheet &stylesheet,
                                         LessRuleset &ruleset) {
  Token token;
  TokenList value;
  UnprocessedStatement* statement;
  CssComment* comment;
  
  while (true) {
    if (tokenizer->getTokenType() == Token::COMMENT) {
      comment = ruleset.createComment();
      comment->setComment(tokenizer->getToken());
      tokenizer->readNextToken();
      skipWhitespace();
    } else if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      token = tokenizer->getToken();
      tokenizer->readNextToken();
      skipWhitespace();
      
      if (parseVariable(value)) {
        ruleset.putVariable(token, value);
        value.clear();
        
      } else if (token == "@media") {
        parseMediaQueryRuleset(token, stylesheet, ruleset);
          
      } else {
        statement = ruleset.createUnprocessedStatement();
  
        statement->getTokens()->push_back(token);
        parseAtRuleValue(*statement->getTokens());
        statement->property_i = 1;
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

void LessParser::parseMediaQueryRuleset(Token &mediatoken,
                                        LessStylesheet &stylesheet,
                                        LessRuleset &parent) {

  MediaQueryRuleset* query = parent.createMediaQuery();
  Selector selector;

  query->setReference(reference);
  
  selector.push_back(mediatoken);
  selector.push_back(Token::BUILTIN_SPACE);

  CssParser::skipWhitespace();

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
                             "{");
  }
  tokenizer->readNextToken();
  skipWhitespace();

  parseRulesetStatements(stylesheet, *query);

  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of media query block ('}')");
  }
  tokenizer->readNextToken();
  skipWhitespace();
}


bool LessParser::parsePropertyVariable(Selector &selector) {
  Token variable = tokenizer->getToken();

  if (tokenizer->getTokenType() != Token::OTHER ||
      variable != "@")
    return false;
  
  if (tokenizer->readNextToken() != Token::BRACKET_OPEN) 
    throw new ParseException(tokenizer->getToken(),
                             "Opening bracket following @");
  variable.append(tokenizer->getToken());
  
  if (tokenizer->readNextToken() != Token::IDENTIFIER) 
    throw new ParseException(tokenizer->getToken(),
                             "Variable inside selector (e.g.: \
@{identifier})");

  variable.append(tokenizer->getToken());
      
  if (tokenizer->readNextToken() != Token::BRACKET_CLOSED)
    throw new ParseException(tokenizer->getToken(),
                               "Closing bracket after variable.");

  variable.append(tokenizer->getToken());
  tokenizer->readNextToken();

  selector.push_back(variable);
  parseWhitespace(selector);
    
  return true;
}


UnprocessedStatement* LessParser::parseRulesetStatement (LessRuleset &ruleset) {
  UnprocessedStatement* statement;
  Selector tokens;
  size_t property_i;
  
  while (parseProperty(tokens) || parsePropertyVariable(tokens)) {}
  
  property_i = tokens.size();

  parseWhitespace(tokens);
  parseSelector(tokens);
  tokens.trim();

  if (tokens.empty())
    return NULL;

  statement = ruleset.createUnprocessedStatement();
  
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

bool LessParser::parseImportStatement(TokenList &statement, LessStylesheet &stylesheet) {
  unsigned int directive = 0;

  // parse directives and strip from statement (the statement becomes a valid
  // css import statement.)
  if (statement.size() >= 4 &&
      statement.front().type == Token::PAREN_OPEN) {
    statement.pop_front();
    statement.ltrim();
    
    directive = parseImportDirective(statement.front());
    statement.pop_front();
    statement.ltrim();
    
    while (statement.size() > 0 && statement.front() == ",") {
      statement.pop_front();
      statement.ltrim();
      
      directive |= parseImportDirective(statement.front());
      statement.pop_front();
      statement.ltrim();
    }

    if (statement.size() > 0 &&
        statement.front().type != Token::PAREN_CLOSED) {
      throw new ParseException(statement, ")");
    } else {
      statement.pop_front();
      statement.ltrim();
    }
  }

  if (statement.size() > 0 &&
      (statement.front().type == Token::URL ||
       statement.front().type == Token::STRING)) {
    return importFile(statement.front(), stylesheet, directive);
        
  } else
    throw new ParseException(statement, "A string with the file path, "
                             "or an import directive.");
}

unsigned int LessParser::parseImportDirective(Token &t) {
  if (t.type != Token::IDENTIFIER) 
    throw new ParseException(t, "an import directive.");
  if (t == "reference")
    return IMPORT_REFERENCE;
  else if (t == "inline")
    return IMPORT_INLINE;
  else if (t == "less")
    return IMPORT_LESS;
  else if (t == "css")
    return IMPORT_CSS;
  else if (t == "once")
    return IMPORT_ONCE;
  else if (t == "multiple")
    return IMPORT_MULTIPLE;
  else if (t == "optional")
    return IMPORT_OPTIONAL;
  else
    throw new ParseException(t, "valid import directive: reference, "
                             "inline, less, css, once, multiple or optional");
}

bool LessParser::importFile(Token uri,
                            LessStylesheet &stylesheet,
                            unsigned int directive) {
  size_t pathend;
  size_t extension_pos;
  std::list<const char*>::iterator i;
  std::string relative_filename;
  char* relative_filename_cpy;
  std::string extension;
    
  if (uri.type == Token::URL) {
    uri = uri.getUrlString();
        
  } else if (uri.type == Token::STRING) {
    uri.removeQuotes();
  } 
        
#ifdef WITH_LIBGLOG
  VLOG(2) << "Import filename: " << uri;
#endif

  // no remote includes
  if (uri.substr(0, 7) == "http://")
    return false;

  pathend = uri.rfind('?');
  if (pathend == std::string::npos)
    pathend = uri.size();

  extension_pos = uri.rfind('.', pathend);
  if (extension_pos == std::string::npos) {
    uri.insert(pathend, ".less");
    pathend += 5;
    extension = "less";
  } else
    extension = uri.substr(extension_pos + 1, pathend);
  
  // don't import css, unless specified with directive
  // don't import if css directive is given
  if ((extension == "css" &&
       !(directive & IMPORT_LESS)) ||
      (directive & IMPORT_CSS)) {
    return false;
  }

  if (!findFile(uri, relative_filename)) {
    if (directive & IMPORT_OPTIONAL)
      return true;
    else {
      throw new ParseException(uri, "existing file",
                               uri.line, uri.column, uri.source);
    }
  }

  if (!(directive & IMPORT_MULTIPLE)) {
    // check if the file has already been imported.
    for (i = sources.begin(); i != sources.end(); i++) {
      if (relative_filename == (*i))
        return true;
    }
  }
  
  ifstream in(relative_filename.c_str());

#ifdef WITH_LIBGLOG
  VLOG(1) << "Opening: " << relative_filename;
#endif

  relative_filename_cpy = new char[relative_filename.length() + 1];
  std::strcpy(relative_filename_cpy, relative_filename.c_str());
              
  sources.push_back(relative_filename_cpy);
  LessTokenizer tokenizer(in, relative_filename_cpy);
  LessParser parser(tokenizer, sources, (directive & IMPORT_REFERENCE));

  parser.includePaths = includePaths;
  
#ifdef WITH_LIBGLOG
  VLOG(2) << "Parsing";
#endif
  
  parser.parseStylesheet(stylesheet);
  in.close();
  return true;
}

bool LessParser::findFile(Token& uri, std::string& filename) {
  ifstream* in;
  size_t pos;
  std::string source;
  std::list<const char*>::iterator i;
  
  source = uri.source;
  pos = source.find_last_of("/\\");

  // if the current stylesheet is outside of the current working
  //  directory then add the directory to the filename.
  if (pos != std::string::npos) {
    filename.append(source.substr(0, pos + 1));
  }
  filename.append(uri);
  
#ifdef WITH_LIBGLOG
  VLOG(2) << "Looking for path: " << filename;
#endif

  in = new ifstream(filename.c_str());
  if (in->good()) {
    in->close();
    return true;
  }

  for (i = includePaths->begin(); i != includePaths->end(); i++) {
    filename.clear();

    filename.append((*i));
    filename.append(uri);

#ifdef WITH_LIBGLOG
    VLOG(2) << "Looking for path: " << filename;
#endif

    in = new ifstream(filename.c_str());
    if (in->good()) {
      in->close();
      return true;
    }
  }
  return false;
}

void LessParser::parseLessMediaQuery(Token &mediatoken,
                                     LessStylesheet &stylesheet) { 
  LessMediaQuery* query = stylesheet.createLessMediaQuery();
  query->setReference(reference);
  
  query->getSelector()->push_back(mediatoken);
  query->getSelector()->push_back(Token::BUILTIN_SPACE);

  CssParser::skipWhitespace();

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
                             "{");
  }
  tokenizer->readNextToken();
  
  skipWhitespace();
  while (parseStatement(*query)) {
    skipWhitespace();
  }
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of media query block ('}')");
  }
  tokenizer->readNextToken();
  skipWhitespace();
}
