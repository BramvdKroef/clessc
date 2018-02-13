#ifndef __less_lessstylesheet_LessAtRule_h__
#define __less_lessstylesheet_LessAtRule_h__

#include "less/stylesheet/AtRule.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/RulesetStatement.h"

#include "less/Token.h"

class LessStylesheet;

class LessAtRule : public AtRule, public RulesetStatement {

public:
  LessAtRule(const Token &keyword);
  virtual ~LessAtRule();

  virtual void process(Stylesheet &s, void* context) const;
  virtual void process(Ruleset& r, void* context) const;
  virtual void write(CssWriter &writer) const;
};

#endif  // __less_lessstylesheet_LessAtRule_h__
