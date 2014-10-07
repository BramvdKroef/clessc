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
#include "ParameterRuleset.h"
#include "LessTokenizer.h"
#include "LessStylesheet.h"
#include "LessMediaQuery.h"

#include <iostream>
#include <fstream>
#include <string>

  
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
  LessParser(CssTokenizer* tokenizer): CssParser(tokenizer) {
  }
  virtual ~LessParser () {
  }

  virtual void parseStylesheet(LessStylesheet* stylesheet);
  
protected:
  /**
   * If an AtRule->getRule() starts with a COLON, add the variable to
   * variables and don't add it to the Stylesheet.
   * 
   */
  bool parseStatement(Stylesheet* stylesheet);
  
  /**
   * If the first token for the rule is a COLON, parse a
   * value. Otherwise parse the usual at-rule.
   */
  bool parseAtRuleOrVariable (LessStylesheet* stylesheet);

  bool parseVariable (TokenList* value);
  bool parseSelector(Selector* selector);
  bool parseSelectorVariable(Selector* selector);
  
  bool parseRuleset (LessStylesheet* stylesheet,
                     Selector* selector = NULL,
                     LessRuleset* parent = NULL);

  bool parseRulesetVariable (LessRuleset* ruleset);
  bool parseRulesetStatement (UnprocessedStatement* statement);

  
  Declaration* parseDeclaration(TokenList* property, TokenList* value);

  bool parseVariable(string keyword, TokenList* value);

  
  bool parseMixin(Selector* selector, Ruleset* ruleset,
                  Stylesheet* stylesheet,
                  ParameterRuleset* parent);
  bool parseParameterMixin(Selector* selector,
                           Ruleset* target,
                           Stylesheet* stylesheet,
                           ParameterRuleset* parent);

  void parseList(list<TokenList*>* list, TokenList* tokens);
  
  void importFile(string file, LessStylesheet* stylesheet);
  void parseLessMediaQuery(LessStylesheet* stylesheet);
private:
    
  TokenList* processValue(TokenList* value);
  
  list<TokenList*>* processArguments(TokenList* arguments);
};

#endif
