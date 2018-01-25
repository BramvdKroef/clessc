#ifndef __less_stylesheet_AtRule_h__
#define __less_stylesheet_AtRule_h__

#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"

#include "less/css/CssWriter.h"

#include "less/Token.h"
#include "less/TokenList.h"

class AtRule : public StylesheetStatement {
private:
  Token keyword;
  TokenList rule;

public:
  AtRule(const Token &keyword);
  virtual ~AtRule();
  void setKeyword(const Token &keyword);
  void setRule(const TokenList &rule);

  const Token &getKeyword() const;
  TokenList &getRule();
  const TokenList &getRule() const;

  virtual void process(Stylesheet &s, void* context) const;
  virtual void write(CssWriter &writer) const;
};

#endif  // __less_stylesheet_AtRule_h__
