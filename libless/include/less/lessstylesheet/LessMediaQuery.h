
#ifndef __LessMediaQuery_h__
#define __LessMediaQuery_h__


#include "../stylesheet/Stylesheet.h"
#include "../stylesheet/StylesheetStatement.h"
#include "../stylesheet/Selector.h"

#include "../css/CssWriter.h"
#include "LessStylesheet.h"
#include "LessRuleset.h"
#include "Mixin.h"
#include "ProcessingContext.h"
#include "Function.h"
#include <list>

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

#endif
