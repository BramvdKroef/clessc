
#ifndef __MixinCall_h__
#define __MixinCall_h__

#include <less/TokenList.h>
#include <less/VariableMap.h>

class Function;
class Mixin;

class MixinCall {
public:
  MixinCall* parent;
  const Function* function;
  VariableMap arguments;
  bool savepoint;

  MixinCall(MixinCall* parent, const Function& function, bool
            savepoint = false);

  const TokenList* getVariable(const std::string &key) const;
  void getFunctions (std::list<const Function*> &functionList,
                     const Mixin& mixin) const;
  bool isInStack(const Function &function) const;
  const VariableMap* getArguments(const Function &function) const;
};

#endif
