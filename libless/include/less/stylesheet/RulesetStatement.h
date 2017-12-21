#ifndef __less_stylesheet_RulesetStatement_h__
#define __less_stylesheet_RulesetStatement_h__

#include "less/stylesheet/CssWritable.h"

class Ruleset;

class RulesetStatement : public CssWritable {
protected:
  Ruleset* ruleset;

public:
  virtual ~RulesetStatement(){};

  virtual void setRuleset(Ruleset* r);
  Ruleset* getRuleset();
  virtual void process(Ruleset& r) = 0;
};

#include "less/stylesheet/Ruleset.h"

#endif  // __less_stylesheet_RulesetStatement_h__
