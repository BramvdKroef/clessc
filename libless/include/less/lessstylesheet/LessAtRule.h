#ifndef __less_lessstylesheet_LessAtRule_h__
#define __less_lessstylesheet_LessAtRule_h__

#include <less/stylesheet/AtRule.h>
#include <less/stylesheet/Stylesheet.h>

#include <less/Token.h>

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

#endif // __less_lessstylesheet_LessAtRule_h__
