#ifndef __less_lessstylesheet_Closure_h__
#define __less_lessstylesheet_Closure_h__

#include "less/lessstylesheet/Function.h"

#include "less/lessstylesheet/Mixin.h"
#include "less/stylesheet/Ruleset.h"

class LessRuleset;
class ProcessingContext;
class MixinCall;

class Closure : public Function {
public:
  const LessRuleset *ruleset;
  const MixinCall *stack;

  Closure(const LessRuleset &ruleset, const MixinCall &stack);

  virtual bool call(MixinArguments &args,
                    Ruleset &target,
                    ProcessingContext &context,
                    bool defaultVal = false) const;
  virtual bool call(MixinArguments &args,
                    Stylesheet &s,
                    ProcessingContext &context,
                    bool defaultVal = false) const;
  virtual void getFunctions(std::list<const Function *> &functionList,
                            const Mixin &mixin,
                            TokenList::const_iterator selector_offset,
                            const ProcessingContext &context) const;
  virtual void getLocalFunctions(std::list<const Function *> &functionList,
                                 const Mixin &mixin,
                                 const ProcessingContext &context) const;

  virtual const LessSelector &getLessSelector() const;

  virtual const TokenList *getVariable(const std::string &key,
                                       const ProcessingContext &context) const;

  bool isInStack(const LessRuleset &ruleset);
};

#endif  // __less_lessstylesheet_Closure_h__
