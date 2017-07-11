
#ifndef __ProcessingContext_h__
#define __ProcessingContext_h__

#include <map>
#include <string>
#include <list>

#include "../TokenList.h"
#include "../VariableMap.h"
#include "../value/ValueScope.h"
#include "../value/ValueProcessor.h"
#include "../lessstylesheet/Extension.h"
#include "MixinCall.h"
#include "Function.h"
#include "Closure.h"

class LessRuleset;
class Function;
class Closure;
class Mixin;
class LessStylesheet;
  
class ProcessingContext: public ValueScope {
private:
  MixinCall* stack;

  ValueProcessor processor;
  std::list<Extension> extensions;

  LessStylesheet* contextStylesheet;
  
  // return values
  std::list<Closure*> closures;
  VariableMap variables;  

public:
  ProcessingContext();

  void setLessStylesheet(LessStylesheet &stylesheet);
  LessStylesheet* getLessStylesheet();
  
  virtual const TokenList* getVariable(const std::string &key) const;
  
  void pushMixinCall(const Function &function, bool savepoint = false);
  void popMixinCall();
  bool isInStack(const Function &function) const;
  VariableMap* getStackArguments();
  bool isStackEmpty() const;
  bool isSavePoint() const;

  void getFunctions (std::list<const Function*> &functionList,
                     const Mixin& mixin) const;
  void getClosures(std::list<const Function*> &closureList,
                   const Mixin &mixin) const;
  
  void saveClosures(std::list<Closure*> &closures);
  void saveVariables(VariableMap &variables);

  void addClosure(const LessRuleset &ruleset);
  void addVariables(const VariableMap &variables);
  
  void addExtension(Extension& extension);
  std::list<Extension>& getExtensions();

  ValueProcessor* getValueProcessor();

  void interpolate(TokenList &tokens);
  void interpolate(std::string &str);
  void processValue(TokenList& value);
  bool validateCondition(TokenList &value);
};

#endif
