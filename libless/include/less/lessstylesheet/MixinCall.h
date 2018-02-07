#ifndef __less_lessstylesheet_MixinCall_h__
#define __less_lessstylesheet_MixinCall_h__

#include "less/TokenList.h"
#include "less/VariableMap.h"

class Function;
class Mixin;
class ProcessingContext;

class MixinCall {
public:
  MixinCall* parent;
  const Function* function;
  VariableMap arguments;
  bool savepoint, important;

  MixinCall(MixinCall* parent,
            const Function& function,
            bool savepoint = false,
            bool important = false);

  const TokenList* getVariable(const std::string& key,
                               const ProcessingContext &context) const;
  void getFunctions(std::list<const Function*>& functionList,
                    const Mixin& mixin,
                    const ProcessingContext &context) const;
  bool isInStack(const Function& function) const;
  const VariableMap* getArguments(const Function& function) const;
};

#endif  // __less_lessstylesheet_MixinCall_h__
