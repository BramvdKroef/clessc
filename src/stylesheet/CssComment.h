
#ifndef __CssComment_h__
#define __CssComment_h__

#include "Stylesheet.h"
#include "StylesheetStatement.h"
#include "RulesetStatement.h"
#include "../css/CssWriter.h"
#include "../Token.h"

class CssComment: public StylesheetStatement, public RulesetStatement {
protected:
  Token comment;

public:
  CssComment();
  CssComment(const Token &comment);
  
  void setComment(const Token &comment);
  Token& getComment();

  virtual void process(Ruleset &r);
  virtual void process(Stylesheet &s);
    
  virtual void write(CssWriter &writer);
};

#endif
