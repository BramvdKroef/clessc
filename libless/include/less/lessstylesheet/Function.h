#ifndef __less_lessstylesheet_Function_h__
#define __less_lessstylesheet_Function_h__

#include <list>

#include "less/TokenList.h"
#include "less/stylesheet/Ruleset.h"

class LessSelector;
class Mixin;
class MixinArguments;
class ProcessingContext;
class LessSelector;
class MixinCall;

class Function {
public:
  virtual bool call(MixinArguments &args,
                    Ruleset &target,
                    ProcessingContext &context,
                    bool defaultVal = false) const = 0;
  virtual bool call(MixinArguments &args,
                    Stylesheet &s,
                    ProcessingContext &context,
                    bool defaultVal = false) const = 0;

  virtual void getFunctions(
      std::list<const Function *> &functionList,
      const Mixin &mixin,
      TokenList::const_iterator selector_offset,
      const ProcessingContext &context) const = 0;

  virtual void getLocalFunctions(std::list<const Function *> &functionList,
                                 const Mixin &mixin,
                                 const ProcessingContext &context) const = 0;

  virtual const TokenList *getVariable(const std::string &key,
                                       const ProcessingContext &context) const = 0;

  virtual const LessSelector& getLessSelector() const = 0;
};

#endif  // __less_lessstylesheet_Function_h__
