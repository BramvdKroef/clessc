#ifndef __less_stylesheet_CssComment_h__
#define __less_stylesheet_CssComment_h__

#include "less/Token.h"
#include "less/css/CssWriter.h"
#include "less/stylesheet/RulesetStatement.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"

class CssComment : public StylesheetStatement, public RulesetStatement {
protected:
  Token comment;

public:
  CssComment();
  CssComment(const Token &comment);

  void setComment(const Token &comment);
  Token &getComment();

  virtual void process(Ruleset &r);
  virtual void process(Stylesheet &s);

  virtual void write(CssWriter &writer);
};

#endif  // __less_stylesheet_CssComment_h__
