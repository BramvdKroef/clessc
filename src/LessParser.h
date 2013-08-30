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
#include "Token.h"
#include "TokenList.h"
#include "ParameterRuleset.h"
#include "value/ValueProcessor.h"
#include "LessTokenizer.h"
#include "ParameterRulesetLibrary.h"

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
  LessParser(CssTokenizer* tokenizer,
             ParameterRulesetLibrary* pRulesets,
             ValueProcessor* valueProcessor): CssParser(tokenizer) {
    this->pRulesets = pRulesets;
    this->valueProcessor = valueProcessor;
  }
  virtual ~LessParser () {
  }
  
protected:
  ValueProcessor* valueProcessor;
  ParameterRulesetLibrary* pRulesets;
  
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
  bool parseAtRuleOrVariable (Stylesheet* stylesheet);

  bool parseRuleset (Stylesheet* stylesheet,
                     Selector* selector = NULL,
                     ParameterRuleset* parent = NULL);

  bool parseRulesetStatement (Stylesheet* stylesheet,
                              Ruleset* ruleset,
                              ParameterRuleset* parent = NULL);

  
  Declaration* parseDeclaration(TokenList* property, TokenList* value);

  bool parseVariable(string keyword);

  bool parseNestedRule(Selector* selector,
                       Ruleset* ruleset,
                       Stylesheet* stylesheet,
                       ParameterRuleset* parent = NULL);
  
  bool parseMixin(Selector* selector, Ruleset* ruleset,
                  Stylesheet* stylesheet,
                  ParameterRuleset* parent);
  bool parseParameterMixin(Selector* selector,
                           Ruleset* target,
                           Stylesheet* stylesheet,
                           ParameterRuleset* parent);

  void importFile(string file, Stylesheet* stylesheet);
private:
    
  TokenList* processValue(TokenList* value);
  
  list<TokenList*>* processArguments(TokenList* arguments);
};

#endif
