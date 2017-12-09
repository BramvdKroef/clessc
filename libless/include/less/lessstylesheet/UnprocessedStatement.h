
#ifndef __UnprocessedStatement_h__
#define __UnprocessedStatement_h__

#include "../stylesheet/Stylesheet.h"
#include "../stylesheet/Ruleset.h"
#include "../stylesheet/Declaration.h"
#include "../stylesheet/RulesetStatement.h"
#include "../css/CssWriter.h"

#include "../TokenList.h"

#include "../value/ValueProcessor.h"

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

#endif
  
