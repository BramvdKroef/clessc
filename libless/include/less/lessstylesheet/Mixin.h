#ifndef __less_lessstylesheet_Mixin_h__
#define __less_lessstylesheet_Mixin_h__

#include <map>
#include <string>
#include <vector>

#include "less/stylesheet/Ruleset.h"
#include "less/stylesheet/Selector.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"

#include "less/css/CssWriter.h"

#include "less/TokenList.h"

#include "less/lessstylesheet/ProcessingContext.h"

class LessStylesheet;
class LessRuleset;
class ProcessingContext;

class Mixin : public StylesheetStatement, public RulesetStatement {
private:
  std::vector<TokenList> arguments;
  std::map<std::string, TokenList> namedArguments;

  LessStylesheet *lessStylesheet;
  LessRuleset *lessRuleset;

public:
  Selector name;

  Mixin();
  Mixin(const Selector &name);
  virtual ~Mixin();

  const TokenList *getArgument(const size_t i) const;
  size_t getArgumentCount() const;

  const TokenList *getArgument(const std::string &name) const;

  void addArgument(TokenList &argument);
  void addArgument(std::string name, TokenList &argument);
  
  bool call(Stylesheet &s,
            ProcessingContext &context,
            Ruleset *ruleset,
            LessRuleset *parent);

  virtual void setLessStylesheet(LessStylesheet &stylesheet);
  LessStylesheet *getLessStylesheet();

  virtual void setLessRuleset(LessRuleset &ruleset);
  LessRuleset *getLessRuleset();

  virtual void process(Ruleset& r);
  virtual void process(Stylesheet &s);
  virtual void write(CssWriter &writer){};
};

#endif  // __less_lessstylesheet_Mixin_h__
