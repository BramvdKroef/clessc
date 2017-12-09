
#ifndef __LessStylesheet_h__
#define __LessStylesheet_h__

#include "../stylesheet/Stylesheet.h"

#include "../TokenList.h"
#include "../Token.h"

#include "LessRuleset.h"
#include "Mixin.h"
#include "UnprocessedStatement.h"
#include "ProcessingContext.h"
#include "LessAtRule.h"

#include <list>
#include <map>
#include <string>

class LessMediaQuery;

class LessStylesheet: public Stylesheet {
private:
  std::list<LessRuleset*> lessrulesets;
  std::list<Closure*> closures;

  VariableMap variables;
  ProcessingContext* context;
  
public:
  LessStylesheet();
  virtual ~LessStylesheet();

  LessRuleset* createLessRuleset();
  Mixin* createMixin();
  LessAtRule* createLessAtRule(const Token &keyword);
  LessMediaQuery* createLessMediaQuery();

  void deleteLessRuleset(LessRuleset &ruleset);
  void deleteMixin(Mixin &mixin);
  
  void setContext(ProcessingContext* context);
  virtual ProcessingContext* getContext();
  
  void putVariable(const std::string &key, const TokenList &value);

  virtual void getFunctions(std::list<const Function*> &functionList,
                            const Mixin &mixin) const;

  virtual const TokenList* getVariable(const std::string &key) const;
  
  virtual void process(Stylesheet &s, ProcessingContext &context);
  void saveReturnValues(ProcessingContext &context);
  
};

#endif
