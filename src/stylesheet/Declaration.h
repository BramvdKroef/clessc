
#ifndef __Declaration_h__
#define __Declaration_h__

#include "RulesetStatement.h"
#include "Ruleset.h"
#include "../css/CssWriter.h"

#include "../Token.h"
#include "../TokenList.h"

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
