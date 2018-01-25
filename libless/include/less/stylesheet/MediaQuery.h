#ifndef __less_stylesheet_MediaQuery_h__
#define __less_stylesheet_MediaQuery_h__

#include "less/stylesheet/Selector.h"
#include "less/stylesheet/Stylesheet.h"

class MediaQuery : public Stylesheet, public StylesheetStatement {
private:
  Selector selector;

public:
  Selector& getSelector();
  const Selector& getSelector() const;
  void setSelector(const Selector& s);

  virtual MediaQuery* createMediaQuery();

  virtual void process(Stylesheet& s, void* context) const;
  virtual void write(CssWriter& writer) const;
};

#endif  // __less_stylesheet_MediaQuery_h__
