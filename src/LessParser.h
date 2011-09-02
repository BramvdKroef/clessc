#ifndef __LessParser_h__
#define __LessParser_h__

#include "CssParser.h"
#include "Stylesheet.h"
#include "Token.h"
#include "TokenList.h"
#include <map>

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
  LessParser(CssTokenizer* tokenizer): CssParser(tokenizer) {}
  
protected:
  map<string, TokenList*> variables;
  vector<Ruleset*> mixins;

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

  Ruleset* parseRuleset (Stylesheet* stylesheet,
                         TokenList* selector = NULL);

  bool parseRulesetStatement (Stylesheet* stylesheet,
                              Ruleset* ruleset);
    
  TokenList* parseValue ();

private:
  TokenList* processValue(TokenList* value);
  bool processVariable (Token* token, TokenList* newvalue);
  bool processDeepVariable (Token* token, Token* nexttoken,
                            TokenList* newvalue);

  void processMixin(Ruleset* parent, Ruleset* mixin);
  void processNestedSelector(TokenList* parent, TokenList* nested);
};

#endif
