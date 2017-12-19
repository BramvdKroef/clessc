#ifndef __less_lessstylesheet_LessStylesheet_h__
#define __less_lessstylesheet_LessStylesheet_h__

#include <list>
#include <map>
#include <string>

#include <less/stylesheet/Stylesheet.h>

#include <less/TokenList.h>
#include <less/Token.h>

#include <less/lessstylesheet/LessRuleset.h>
#include <less/lessstylesheet/Mixin.h>
#include <less/lessstylesheet/UnprocessedStatement.h>
#include <less/lessstylesheet/ProcessingContext.h>
#include <less/lessstylesheet/LessAtRule.h>

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

#endif // __less_lessstylesheet_LessStylesheet_h__
