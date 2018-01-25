#ifndef __less_lessstylesheet_LessDeclaration_h__
#define __less_lessstylesheet_LessDeclaration_h__

#include "less/stylesheet/Declaration.h"
#include "less/stylesheet/Ruleset.h"

class LessRuleset;

class LessDeclaration : public Declaration {
  LessRuleset *lessRuleset;

public:
  void setLessRuleset(LessRuleset &r);
  LessRuleset *getLessRuleset();

  virtual void process(Ruleset &r, void* context) const;

};

#endif  // __less_lessstylesheet_LessDeclaration_h__
