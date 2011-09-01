#ifndef __LessParser_h__
#define __LessParser_h__

#include "CssParser.h"
#include "Stylesheet.h"
#include "Token.h"
#include <map>
#include <vector>

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
  map<string, vector<Token*>*> variables;
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

  Ruleset* parseRuleset ();

  bool parseRulesetStatement (Ruleset* ruleset);
    
  vector<Token*>* parseValue ();

private:
  vector<Token*>* processValue(vector<Token*>* value);
  bool processVariable (Token* token, vector<Token*>* newvalue);
  bool processDeepVariable (Token* token, Token* nexttoken,
                            vector<Token*>* newvalue);

  void processNestedRuleset (Ruleset* parent, Ruleset* nested);
  void processMixin(Ruleset* parent, Ruleset* mixin);
};

#endif
