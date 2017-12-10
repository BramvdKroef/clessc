#ifndef __less_lessstylesheet_LessMediaQuery_h__
#define __less_lessstylesheet_LessMediaQuery_h__

#include <list>

#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"
#include "less/stylesheet/Selector.h"

#include "less/css/CssWriter.h"
#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/Mixin.h"
#include "less/lessstylesheet/ProcessingContext.h"
#include "less/lessstylesheet/Function.h"

class LessMediaQuery: public LessStylesheet, public StylesheetStatement {
private:
  Selector selector;
  LessStylesheet* parent;
  
public:
  LessMediaQuery();
  virtual ~LessMediaQuery();
  
  Selector* getSelector();
  void setSelector(const Selector &s);

  virtual void setLessStylesheet(LessStylesheet &parent);
  LessStylesheet* getLessStylesheet() const;

  virtual void getFunctions(std::list<const Function*> &functionList,
                            const Mixin &mixin) const;
  virtual const TokenList* getVariable(const std::string &key) const;
  
  virtual ProcessingContext* getContext();
  virtual void process(Stylesheet &s);
  virtual void write(CssWriter &writer);
};

#endif // __less_lessstylesheet_LessMediaQuery_h__
