#ifndef __less_lessstylesheet_ProcessingContext_h__
#define __less_lessstylesheet_ProcessingContext_h__

#include <list>
#include <map>
#include <string>

#include "less/TokenList.h"
#include "less/VariableMap.h"
#include "less/lessstylesheet/Closure.h"
#include "less/lessstylesheet/Extension.h"
#include "less/lessstylesheet/Function.h"
#include "less/lessstylesheet/MixinCall.h"
#include "less/value/ValueProcessor.h"
#include "less/value/ValueScope.h"

class LessRuleset;
class Function;
class Closure;
class Mixin;
class LessStylesheet;

class ProcessingContext : public ValueScope {
private:
  MixinCall *stack;

  ValueProcessor processor;
  std::list<std::list<Extension>*> extensions;

  const LessStylesheet *contextStylesheet;

  // return values
  std::map<const Function*, std::list<Closure *> > closures;
  std::map<const Function*, VariableMap> variables;
  std::list<Closure *> base_closures;
  VariableMap base_variables;
public:
  ProcessingContext();
  virtual ~ProcessingContext();

  void setLessStylesheet(const LessStylesheet &stylesheet);
  const LessStylesheet *getLessStylesheet() const;

  virtual const TokenList *getVariable(const std::string &key) const;

  const TokenList *getFunctionVariable(const std::string &key,
                                       const Function* function) const;

  const TokenList *getBaseVariable (const std::string &key) const;
  
  
  void pushMixinCall(const Function &function,
                     bool savepoint = false,
                     bool important = false);
  void popMixinCall();
  bool isInStack(const Function &function) const;
  VariableMap *getStackArguments() const;
  VariableMap *getStackArguments(const Function *function) const;
  bool isStackEmpty() const;
  bool isSavePoint() const;
  const Function* getSavePoint() const;
  bool isImportant() const;

  void getFunctions(std::list<const Function *> &functionList,
                    const Mixin &mixin) const;

  const std::list<Closure *> *getClosures(const Function *function) const ;
  const std::list<Closure *> *getBaseClosures() const;
  
  void addClosure(const LessRuleset &ruleset);
  void addVariables(const VariableMap &variables);

  void pushExtensionScope(std::list<Extension> &scope);
  void popExtensionScope();
  
  void addExtension(Extension &extension);
  std::list<Extension> *getExtensions();

  ValueProcessor *getValueProcessor();

  void interpolate(Selector &selector) const;
  void interpolate(TokenList &tokens) const;
  void interpolate(std::string &str) const;
  void processValue(TokenList &value) const;
  bool validateCondition(const TokenList &value,
                         bool defaultVal = false) const;
};

#endif  // __less_lessstylesheet_ProcessingContext_h__
