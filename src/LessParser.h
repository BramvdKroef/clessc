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

#ifndef __LessParser_h__
#define __LessParser_h__

#include "CssParser.h"
#include "Stylesheet.h"
#include "LessRuleset.h"
#include "Token.h"
#include "TokenList.h"
#include "LessTokenizer.h"
#include "LessStylesheet.h"
#include "LessMediaQuery.h"
#include "MediaQueryRuleset.h"

#include <iostream>
#include <fstream>
#include <string>
#include <list>
  
/**
 * Extends the css spec with these parts:
 * * Variables
 *  at-rule: ATKEYWORD S* [ ':' value ';' S* | any* [ block | ';' S* ] ];
 *  
 * * Mixins
 *  ruleset: selector? '{' S* [declaration | selector]?
 *               [ ';' S* [declaration | selector]? ]* '}' S*;
 *  
 * * Parametric Mixins
 *  ruleset: [selector parameters?]? '{' S* declaration?
 *              [ ';' S* declaration? ]* '}' S*;
 * parameters: '(' parameter? [ ',' S* parameter ]* ')'
 * parameter: ATKEYWORD [ ':' S* value ]? S*
 * 
 * * Nested Rules
 *  ruleset: [selector parameters]? '{' S* r-statement? '}' S*;
 *  r-statement: [ declaration [';' S* r-statement ]?
 *                | ruleset r-statement? ]
 * * Operations
 *
 *  value: value-old [ [  '+' | '-' | '*' | '/' ] value-old ]*
 *  
 */
class LessParser: public CssParser {
public:
  static const unsigned int IMPORT_REFERENCE = 1,
    IMPORT_INLINE = 2,
    IMPORT_LESS = 4,
    IMPORT_CSS = 6,
    IMPORT_ONCE = 12,
    IMPORT_MULTIPLE = 24,
    IMPORT_OPTIONAL = 48;

  LessParser(CssTokenizer &tokenizer,
             std::list<const char*> &source_files):
    CssParser(tokenizer),
    sources(source_files),
    reference(false) {
  }
  LessParser(CssTokenizer &tokenizer,
             std::list<const char*> &source_files,
             bool isreference):
    CssParser(tokenizer),
    sources(source_files),
    reference(isreference) {
  }
  virtual ~LessParser () {
  }

  virtual void parseStylesheet(LessStylesheet &stylesheet);
  
protected:
  std::list<const char*> &sources;
  bool reference;
  
  /**
   * If an AtRule->getRule() starts with a COLON, add the variable to
   * variables and don't add it to the Stylesheet.
   * 
   */
  bool parseStatement(Stylesheet &stylesheet);
  
  /**
   * If the first token for the rule is a COLON, parse a
   * value. Otherwise parse the usual at-rule.
   */
  bool parseAtRuleOrVariable (LessStylesheet &stylesheet);

  bool parseVariable (TokenList &value);
  bool parseSelector(Selector &selector);
  bool parseSelectorVariable(Selector &selector);
  
  bool parseRuleset (LessStylesheet &stylesheet,
                     Selector &selector,
                     LessRuleset* parent = NULL);

  void parseRulesetStatements (LessStylesheet &stylesheet,
                               LessRuleset &ruleset);

  void parseMediaQueryRuleset(Token &mediatoken,
                              LessStylesheet &stylesheet,
                              LessRuleset &parent);
  UnprocessedStatement* parseRulesetStatement (LessRuleset &parent);

  
  Declaration* parseDeclaration(TokenList &property, TokenList &value);

  bool parseVariable(std::string &keyword, TokenList &value);

  
  void parseList(std::list<TokenList*>* list, TokenList* tokens);

  bool parseImportStatement(TokenList &statement, LessStylesheet &stylesheet);
  unsigned int parseImportDirective(Token &t);
  bool importFile(Token uri, LessStylesheet &stylesheet, unsigned int directive);

  void parseLessMediaQuery(Token &mediatoken,
                           LessStylesheet &stylesheet);
private:
    
  TokenList* processValue(TokenList* value);
  
  std::list<TokenList*>* processArguments(TokenList* arguments);
};

#endif
