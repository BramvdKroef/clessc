
#ifndef __MediaQuery_h__
#define __MediaQuery_h__

#include "Stylesheet.h"
#include "Selector.h"

class MediaQuery: public Stylesheet, public StylesheetStatement {
private:
  Selector selector;

public:
  Selector& getSelector();
  void setSelector(const Selector& s);

  virtual MediaQuery* createMediaQuery();
  
  virtual void process(Stylesheet& s);
  virtual void write(CssWriter &writer);
};


#endif
