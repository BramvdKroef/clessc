#ifndef __less_lessstylesheet_Function_h__
#define __less_lessstylesheet_Function_h__

#include <list>

#include "less/stylesheet/Ruleset.h"
#include "less/TokenList.h"

class LessSelector;
class Mixin;
class ProcessingContext;
class LessSelector;
class MixinCall;

class Function {
public:
  virtual bool call(Mixin& mixin, Ruleset &target,
                      ProcessingContext& context) const = 0;
  virtual bool call(Mixin& mixin, Stylesheet &s,
                    ProcessingContext& context) const = 0;
  virtual void getFunctions(std::list<const Function*> &functionList,
                            const Mixin &mixin,
                            TokenList::const_iterator selector_offset)
  const = 0;

  virtual void getLocalFunctions(std::list<const Function*> &functionList,
                                 const Mixin &mixin) const = 0;

  virtual const TokenList* getVariable(const std::string &key) const = 0;
  virtual const TokenList* getInheritedVariable(const std::string
  &key, const MixinCall &stack) const
  = 0;

  void saveReturnValues(ProcessingContext &context);
  
  virtual LessSelector* getLessSelector() const = 0;
 
};

#endif // __less_lessstylesheet_Function_h__
