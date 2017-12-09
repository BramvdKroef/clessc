
#ifndef __LessRuleset_h__
#define __LessRuleset_h__

#include "../stylesheet/Stylesheet.h"
#include "../stylesheet/Ruleset.h"
#include "../stylesheet/Selector.h"

#include "../css/ParseException.h"
#include "../value/ValueProcessor.h"

#include "../Token.h"
#include "../TokenList.h"
#include "../VariableMap.h"

#include "Function.h"
#include "UnprocessedStatement.h"
#include "LessSelector.h"
#include "Mixin.h"
#include "ProcessingContext.h"

#include <map>
#include <list>
#include <string>

class LessStylesheet;
class MediaQueryRuleset;
class Closure;

class LessRuleset: public Ruleset, Function {
  
protected:
  VariableMap variables;  
  list<LessRuleset*> nestedRules;
  std::list<Closure*> closures;

  list<UnprocessedStatement*> unprocessedStatements;

  LessRuleset* parent;
  LessStylesheet* lessStylesheet;
  LessSelector* selector;

  ProcessingContext* context;

  void processVariables();
  void insertNestedRules(Stylesheet &s, Selector* prefix,
                         ProcessingContext &context) const;

  void addClosures(ProcessingContext &context) const;
  
public:
  LessRuleset();
  LessRuleset(const Selector &selector);
  virtual ~LessRuleset();

  virtual void setSelector(const Selector &selector);
  virtual LessSelector* getLessSelector() const;

  UnprocessedStatement* createUnprocessedStatement();
  LessRuleset* createNestedRule();
  MediaQueryRuleset* createMediaQuery();

  void deleteNestedRule(LessRuleset &ruleset);
  void deleteUnprocessedStatement(UnprocessedStatement
                                  &statement);
  
  const list<UnprocessedStatement*>& getUnprocessedStatements() const;
  const list<LessRuleset*>& getNestedRules() const;

  void putVariable(const std::string &key, const TokenList &value);
  VariableMap& getVariables();

  const TokenList* getVariable(const std::string &key) const;
  const TokenList* getInheritedVariable(const std::string &key,
                                        const MixinCall &stack) const;

  const list<Closure*>& getClosures() const;
  
  void setParent(LessRuleset* r);
  LessRuleset* getParent() const;

  void setLessStylesheet(LessStylesheet &stylesheet);
  LessStylesheet* getLessStylesheet() const;

  ProcessingContext* getContext();
  
  void processExtensions(ProcessingContext &context,
                         Selector* prefix);
  
  virtual bool call(Mixin &mixin, Ruleset &target,
                      ProcessingContext& context) const;
  virtual bool call(Mixin &mixin, Stylesheet &s,
                    ProcessingContext& context) const;

  virtual void processStatements(Ruleset &target, ProcessingContext& context) const;
  void processStatements(Stylesheet &target,
                                      ProcessingContext& context) const;
  virtual void process(Stylesheet &s);
  virtual void process(Stylesheet &s, Selector* prefix,
                       ProcessingContext &context);

  virtual void getFunctions(list<const Function*> &functionList,
                            const Mixin &mixin,
                            TokenList::const_iterator selector_offset) const;

  void saveReturnValues(ProcessingContext &context);
  /**
   * Look for a ruleset inside this ruleset and scope up to
   * getParent(), or getLessStylesheet() if getParent() is NULL.
   */
  void getLocalFunctions(std::list<const Function*> &functionList,
                         const Mixin &mixin) const;
  void getLocalFunctions(std::list<const Function*> &functionList,
                         const Mixin &mixin,
                         const LessRuleset *exclude = NULL) const;

  bool matchConditions(ProcessingContext &context) const;
  bool putArguments(const Mixin &mixin,
                    VariableMap &scope) const;
};

#endif
