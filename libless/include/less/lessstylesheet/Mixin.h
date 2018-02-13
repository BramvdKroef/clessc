#ifndef __less_lessstylesheet_Mixin_h__
#define __less_lessstylesheet_Mixin_h__


#include "less/stylesheet/Ruleset.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"
#include "less/stylesheet/RulesetStatement.h"

#include "less/lessstylesheet/MixinArguments.h"

class LessStylesheet;
class LessRuleset;
class ProcessingContext;

class Mixin : public StylesheetStatement, public RulesetStatement {
private:

  const LessStylesheet *lessStylesheet;
  const LessRuleset *lessRuleset;

  bool important;
  
public:
  TokenList name;
  MixinArguments arguments;

  Mixin(const TokenList &name, const LessStylesheet &parent);
  Mixin(const TokenList &name, const LessRuleset &parent);
  virtual ~Mixin();

  bool call(ProcessingContext &context,
            Ruleset *ruleset,
            Stylesheet *stylesheet) const;

  void setImportant(bool b);
  bool isImportant() const;
  
  const LessStylesheet *getLessStylesheet() const;

  const LessRuleset *getLessRuleset() const;

  virtual void process(Ruleset& r, void* context) const;
  virtual void process(Stylesheet &s, void *context) const;
  virtual void write(CssWriter &writer) const;
};

#endif  // __less_lessstylesheet_Mixin_h__
