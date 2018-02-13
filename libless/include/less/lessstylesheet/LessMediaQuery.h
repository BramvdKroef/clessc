#ifndef __less_lessstylesheet_LessMediaQuery_h__
#define __less_lessstylesheet_LessMediaQuery_h__

#include <list>

#include "less/stylesheet/Selector.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"

#include "less/css/CssWriter.h"
#include "less/lessstylesheet/Function.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/Mixin.h"
#include "less/lessstylesheet/ProcessingContext.h"

class LessMediaQuery : public LessStylesheet, public StylesheetStatement {
private:
  TokenList selector;
  const LessStylesheet *parent;

public:
  LessMediaQuery(const TokenList &selector, const LessStylesheet &parent);
  virtual ~LessMediaQuery();

  TokenList &getSelector();
  const TokenList &getSelector() const;
  void setSelector(const TokenList &s);

  const LessStylesheet &getLessStylesheet() const;

  virtual void getFunctions(std::list<const Function *> &functionList,
                            const Mixin &mixin,
                            const ProcessingContext &context) const;
  virtual const TokenList *getVariable(const std::string &key,
                                       const ProcessingContext &context) const;

  virtual void process(Stylesheet &s, void* context) const;
  virtual void write(CssWriter &writer) const;
};

#endif  // __less_lessstylesheet_LessMediaQuery_h__
