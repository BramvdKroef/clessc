#ifndef __less_lessstylesheet_UnprocessedStatement_h__
#define __less_lessstylesheet_UnprocessedStatement_h__

#include <less/stylesheet/Stylesheet.h>
#include <less/stylesheet/Ruleset.h>
#include <less/stylesheet/Declaration.h>
#include <less/stylesheet/RulesetStatement.h>
#include <less/css/CssWriter.h>

#include <less/TokenList.h>

#include <less/value/ValueProcessor.h>

#include <iterator>

class LessRuleset;

class UnprocessedStatement: public RulesetStatement {
private:
  Selector tokens;
  LessRuleset* lessRuleset;
  
protected:
  bool processDeclaration (Declaration &declaration);
  bool isDeclaration();
    
public:
  size_t property_i;
  
  UnprocessedStatement();
  ~UnprocessedStatement() {}

  Selector* getTokens();

  void getProperty(TokenList &tokens);
  void getValue(TokenList &tokens);

  void setLessRuleset(LessRuleset &r);
  LessRuleset* getLessRuleset();

  bool isExtends();
  bool getExtension(TokenList &extension);
  
  void process(Stylesheet &s);
    
  virtual void process(Ruleset &r) ;
  virtual void write(CssWriter &css) {};
};

#endif // __less_lessstylesheet_UnprocessedStatement_h__
