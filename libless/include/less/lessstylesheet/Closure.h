#ifndef __less_lessstylesheet_Closure_h__
#define __less_lessstylesheet_Closure_h__

#include "less/lessstylesheet/Function.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/Mixin.h"
#include "less/stylesheet/Ruleset.h"

class ProcessingContext;
class MixinCall;

class Closure : public Function {
public:
  const LessRuleset *ruleset;
  const MixinCall *stack;

  Closure(const LessRuleset &ruleset, const MixinCall &stack);

  virtual bool call(Mixin &mixin,
                    Ruleset &target,
                    ProcessingContext &context) const;
  virtual bool call(Mixin &mixin,
                    Stylesheet &s,
                    ProcessingContext &context) const;
  virtual void getFunctions(std::list<const Function *> &functionList,
                            const Mixin &mixin,
                            TokenList::const_iterator selector_offset) const;
  virtual void getLocalFunctions(list<const Function *> &functionList,
                                 const Mixin &mixin) const;

  virtual LessSelector *getLessSelector() const;

  virtual const TokenList *getVariable(const std::string &key) const;
  virtual const TokenList *getInheritedVariable(const std::string &key,
                                                const MixinCall &stack) const;

  bool isInStack(const LessRuleset &ruleset);
};

#endif  // __less_lessstylesheet_Closure_h__
