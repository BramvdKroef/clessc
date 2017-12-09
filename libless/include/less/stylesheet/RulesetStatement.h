
#ifndef __RulesetStatement_h__
#define __RulesetStatement_h__

#include <less/stylesheet/CssWritable.h>

class Ruleset;

class RulesetStatement :  public CssWritable {
protected:
  Ruleset* ruleset;
public:
  virtual ~RulesetStatement() {};

  virtual void setRuleset(Ruleset *r);
  Ruleset* getRuleset();
  virtual void process(Ruleset &r) = 0;
};

#include <less/stylesheet/Ruleset.h>

#endif
