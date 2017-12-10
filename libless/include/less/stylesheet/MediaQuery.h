#ifndef __less_stylesheet_MediaQuery_h__
#define __less_stylesheet_MediaQuery_h__

#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/Selector.h"

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

#endif // __less_stylesheet_MediaQuery_h__
