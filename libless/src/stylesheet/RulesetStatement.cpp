#include "less/stylesheet/RulesetStatement.h"

void RulesetStatement::setRuleset(Ruleset* r) {
  ruleset = r;
}
Ruleset* RulesetStatement::getRuleset() {
  return ruleset;
}
