#ifndef __less_lessstylesheet_LessRuleset_h__
#define __less_lessstylesheet_LessRuleset_h__

#include <list>
#include <map>
#include <string>

#include "less/stylesheet/Ruleset.h"
#include "less/stylesheet/Selector.h"
#include "less/stylesheet/Stylesheet.h"

#include "less/css/ParseException.h"
#include "less/value/ValueProcessor.h"

#include "less/Token.h"
#include "less/TokenList.h"
#include "less/VariableMap.h"

#include "less/lessstylesheet/Function.h"
#include "less/lessstylesheet/LessSelector.h"
#include "less/lessstylesheet/Mixin.h"
#include "less/lessstylesheet/MixinArguments.h"
#include "less/lessstylesheet/ProcessingContext.h"
#include "less/lessstylesheet/LessDeclaration.h"
#include "less/lessstylesheet/LessAtRule.h"

class LessStylesheet;
class MediaQueryRuleset;
class Closure;

class LessRuleset : public Ruleset, public Function {
protected:
  VariableMap variables;
  std::list<LessRuleset *> nestedRules;
  std::list<Closure *> closures;
  std::list<Extension> extensions;

  std::list<StylesheetStatement *> stylesheetStatements;
  
  std::list<LessDeclaration *> lessDeclarations;
  std::list<Mixin *> mixins;
  std::list<LessAtRule *> lessAtRules;

  const LessRuleset *parent;
  const LessStylesheet *lessStylesheet;
  LessSelector *selector;

  ProcessingContext *context;

  void processVariables();
  void insertNestedRules(Stylesheet &s,
                         const Selector *prefix,
                         ProcessingContext &context) const;

  void addClosures(ProcessingContext &context) const;

  bool call(MixinArguments& args,
            ProcessingContext& context,
            Ruleset* ruleset,
            Stylesheet* stylesheet,
            bool defaultVal = false) const;

  void mergeDeclarations(Ruleset &ruleset, Declaration* merge = NULL) const;
public:
  LessRuleset(LessSelector &selector,
              const LessRuleset& parent);
  LessRuleset(LessSelector &selector,
              const LessStylesheet& parent);
  
  virtual ~LessRuleset();

  virtual const LessSelector& getLessSelector() const;

  void addExtension(Extension &extension);
    
  LessDeclaration *createLessDeclaration();
  Mixin *createMixin(const TokenList &selector);
  LessAtRule *createLessAtRule(const Token& keyword);
  LessRuleset *createNestedRule(LessSelector &selector);
  MediaQueryRuleset *createMediaQuery(TokenList &selector);

  void deleteNestedRule(LessRuleset &ruleset);


  const list<LessDeclaration *> &getLessDeclarations() const;
  const list<Mixin *> &getMixins() const;
  const list<LessAtRule *> &getLessAtRules() const;
  const list<StylesheetStatement *> &getStylesheetStatements() const;

  const list<LessRuleset *> &getNestedRules() const;

  void putVariable(const std::string &key, const TokenList &value);
  VariableMap &getVariables();

  const TokenList *getVariable(const std::string &key) const;
  const TokenList *getInheritedVariable(const std::string &key,
                                        const MixinCall &stack) const;

  const list<Closure *> &getClosures() const;

  void setParent(const LessRuleset *r);
  const LessRuleset *getParent() const;

  void setLessStylesheet(const LessStylesheet &stylesheet);
  const LessStylesheet *getLessStylesheet() const;

  void processExtensions(ProcessingContext &context,
                         const Selector *prefix) const;
  void processInlineExtensions(ProcessingContext& context,
                               const Selector &selector) const;

  virtual bool call(MixinArguments &args,
                    Ruleset &target,
                    ProcessingContext &context,
                    bool defaultVal = false) const;
  virtual bool call(MixinArguments &args,
                    Stylesheet &s,
                    ProcessingContext &context,
                    bool defaultVal = false) const;

  virtual void processStatements(Ruleset &target,
                                 void* context) const;
  void processStatements(Stylesheet &target, void* context) const;
  virtual void process(Stylesheet &s, void* context) const;
  virtual void process(Stylesheet &s,
                       const Selector *prefix,
                       ProcessingContext &context) const;

  const TokenList* getVariable(const std::string& key,
                               const ProcessingContext &context) const ;

  virtual void getFunctions(list<const Function *> &functionList,
                            const Mixin &mixin,
                            TokenList::const_iterator selector_offset,
                            const ProcessingContext &context) const;

  /**
   * Look for a ruleset inside this ruleset and scope up to
   * getParent(), or getLessStylesheet() if getParent() is NULL.
   */
  void getLocalFunctions(std::list<const Function *> &functionList,
                         const Mixin &mixin,
                         const ProcessingContext &context) const;
  void getLocalFunctions(std::list<const Function *> &functionList,
                         const Mixin &mixin,
                         const LessRuleset *exclude,
                         const ProcessingContext &context) const;

  bool matchConditions(const ProcessingContext &context,
                       bool defaultVal = false) const;
  bool putArguments(MixinArguments &args, VariableMap &scope) const;
};

#endif  // __less_lessstylesheet_LessRuleset_h__
