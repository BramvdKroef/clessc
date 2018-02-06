#ifndef __less_lessstylesheet_LessStylesheet_h__
#define __less_lessstylesheet_LessStylesheet_h__

#include <list>
#include <map>
#include <string>

#include "less/stylesheet/Stylesheet.h"

#include "less/Token.h"
#include "less/TokenList.h"

#include "less/lessstylesheet/LessAtRule.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/Mixin.h"
#include "less/lessstylesheet/ProcessingContext.h"

class LessMediaQuery;

class LessStylesheet : public Stylesheet {
private:
  std::list<LessRuleset *> lessrulesets;
  std::list<Closure *> closures;

  VariableMap variables;

public:
  LessStylesheet();
  virtual ~LessStylesheet();

  LessRuleset *createLessRuleset(const LessSelector &selector);
  
  Mixin *createMixin(const Selector &selector);
  LessAtRule *createLessAtRule(const Token &keyword);

  LessMediaQuery *createLessMediaQuery(const Selector &selector);

  void deleteLessRuleset(LessRuleset &ruleset);
  void deleteMixin(Mixin &mixin);

  void putVariable(const std::string &key, const TokenList &value);

  virtual void getFunctions(std::list<const Function *> &functionList,
                            const Mixin &mixin,
                            const ProcessingContext &context) const;

  const TokenList *getVariable(const std::string &key) const;
  virtual const TokenList *getVariable(const std::string &key,
                                       const ProcessingContext &context) const;

  virtual void process(Stylesheet &s, void *context) const;
};

#endif  // __less_lessstylesheet_LessStylesheet_h__
