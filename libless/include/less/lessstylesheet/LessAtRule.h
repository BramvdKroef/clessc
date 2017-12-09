
#ifndef __LessAtRule_h__
#define __LessAtRule_h__

#include "../stylesheet/AtRule.h"
#include "../stylesheet/Stylesheet.h"

#include "../Token.h"

class LessStylesheet;

class LessAtRule: public AtRule {
private:
  LessStylesheet *lessStylesheet;

public:
  LessAtRule(const Token& keyword);
  virtual ~LessAtRule();

  void setLessStylesheet(LessStylesheet &stylesheet);
  LessStylesheet* getLessStylesheet();

  virtual void process(Stylesheet &s);
};

#endif
