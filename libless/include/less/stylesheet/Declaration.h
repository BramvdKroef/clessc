
#ifndef __Declaration_h__
#define __Declaration_h__
#include <less/stylesheet/RulesetStatement.h>
#include <less/stylesheet/Ruleset.h>
#include <less/css/CssWriter.h>

#include <less/Token.h>
#include <less/TokenList.h>

class Declaration: public RulesetStatement {
private:
  Token property;
  TokenList value;
  
public:
  Declaration();
  Declaration(const Token &property);
  virtual ~Declaration();
  void setProperty(const Token &property);
  void setValue(const TokenList &value);
  
  Token& getProperty();
  TokenList& getValue();

  virtual void process(Ruleset &r);
  virtual void write(CssWriter &writer);
};

#endif
