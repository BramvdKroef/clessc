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
  const Token &getComment() const;

  virtual void process(Ruleset &r, void* context) const;
  virtual void process(Stylesheet &s, void* context) const;

  virtual void write(CssWriter &writer) const;
};

#endif  // __less_stylesheet_CssComment_h__
