#ifndef __LessParser_h__
#define __LessParser_h__

#include "CssParser.h"
#include "Stylesheet.h"
#include "Token.h"
#include "TokenList.h"
#include "ParameterRuleset.h"
#include "ValueProcessor.h"

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
    valueProcessor = new ValueProcessor();
  }
  virtual ~LessParser () {
    delete valueProcessor;
  }
  
protected:
  vector<ParameterRuleset*> parameterRulesets;
  ValueProcessor* valueProcessor;
  
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
                         TokenList* selector = NULL);

  bool parseRulesetStatement (Stylesheet* stylesheet,
                              Ruleset* ruleset);

  Declaration* parseDeclaration(string* property);

  void parseMixin(TokenList* selector, Ruleset* ruleset,
                  Stylesheet* stylesheet);
  bool processParameterMixin(TokenList* selector, Ruleset* parent);
  
  ParameterRuleset* getParameterRuleset(TokenList* selector);

  TokenList* parseValue ();
  
private:
    
  TokenList* processValue(TokenList* value);
  
  void processParameterRuleset(ParameterRuleset* ruleset);
  bool processParameter(TokenList* selector,
                        ParameterRuleset* ruleset);

  list<TokenList*>* processArguments(TokenList* arguments);
};

#endif
