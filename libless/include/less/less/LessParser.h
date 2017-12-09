#ifndef __LessParser_h__
#define __LessParser_h__

#include <iostream>
#include <fstream>
#include <string>
#include <list>
  
#include <less/css/CssParser.h>
#include <less/css/CssTokenizer.h>
#include <less/stylesheet/Stylesheet.h>
#include <less/lessstylesheet/LessRuleset.h>
#include <less/lessstylesheet/LessStylesheet.h>
#include <less/lessstylesheet/LessMediaQuery.h>
#include <less/lessstylesheet/MediaQueryRuleset.h>

#include <less/Token.h>
#include <less/TokenList.h>

#include <less/less/LessTokenizer.h>

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
    IMPORT_CSS = 8,
    IMPORT_ONCE = 16,
    IMPORT_MULTIPLE = 32,
    IMPORT_OPTIONAL = 64;

  std::list<const char*>* includePaths;
  
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
   * Skip comments only if they are LESS comments, not CSS comments.
   */ 
  virtual void skipWhitespace();
  
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

  bool parseAtRuleValue(TokenList &rule);

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

  bool parsePropertyVariable(Selector &selector);
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

  bool findFile(Token& uri, std::string& filename);
};

#endif
