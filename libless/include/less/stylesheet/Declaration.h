#ifndef __less_stylesheet_Declaration_h__
#define __less_stylesheet_Declaration_h__

#include "less/css/CssWriter.h"
#include "less/stylesheet/Ruleset.h"
#include "less/stylesheet/RulesetStatement.h"

#include "less/Token.h"
#include "less/TokenList.h"

class Declaration : public RulesetStatement {
protected:
  Token property;
  TokenList value;

public:
  Declaration();
  Declaration(const Token &property);
  virtual ~Declaration();
  void setProperty(const Token &property);
  void setValue(const TokenList &value);

  Token &getProperty();
  const Token &getProperty() const;
  TokenList &getValue();
  const TokenList &getValue() const;

  virtual void process(Ruleset &r, void* context) const;
  virtual void write(CssWriter &writer) const;
};

#endif  // __less_stylesheet_Declaration_h__
