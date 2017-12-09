
#ifndef __AtRule_h__
#define __AtRule_h__
#include <less/stylesheet/Stylesheet.h>
#include <less/stylesheet/StylesheetStatement.h>

#include <less/css/CssWriter.h>

#include <less/Token.h>
#include <less/TokenList.h>

class AtRule: public StylesheetStatement {
private:
  Token keyword;
  TokenList rule;

public:
  AtRule(const Token& keyword);
  virtual ~AtRule();
  void setKeyword (const Token &keyword);
  void setRule(const TokenList &rule);

  Token& getKeyword();
  TokenList& getRule();

  virtual void process(Stylesheet &s);
  virtual void write(CssWriter &writer);
};


#endif
