#ifndef __less_lessstylesheet_Mixin_h__
#define __less_lessstylesheet_Mixin_h__


#include "less/stylesheet/Ruleset.h"
#include "less/stylesheet/Selector.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"
#include "less/stylesheet/RulesetStatement.h"

#include "less/lessstylesheet/MixinArguments.h"

class LessStylesheet;
class LessRuleset;
class ProcessingContext;

class Mixin : public StylesheetStatement, public RulesetStatement {
private:

  LessStylesheet *lessStylesheet;
  LessRuleset *lessRuleset;

public:
  Selector name;
  MixinArguments arguments;
  
  Mixin();
  Mixin(const Selector &name);
  virtual ~Mixin();

  bool call(Stylesheet &s,
            ProcessingContext &context,
            Ruleset *ruleset,
            LessRuleset *parent) const;

  virtual void setLessStylesheet(LessStylesheet &stylesheet);
  LessStylesheet *getLessStylesheet() const;

  virtual void setLessRuleset(LessRuleset &ruleset);
  LessRuleset *getLessRuleset();

  virtual void process(Ruleset& r);
  virtual void process(Stylesheet &s);
  virtual void write(CssWriter &writer){};
};

#endif  // __less_lessstylesheet_Mixin_h__
