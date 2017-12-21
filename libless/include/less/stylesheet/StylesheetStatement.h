#ifndef __less_stylesheet_StylesheetStatement_h__
#define __less_stylesheet_StylesheetStatement_h__

#include "less/stylesheet/CssWritable.h"
#include "less/stylesheet/Stylesheet.h"

class StylesheetStatement : public CssWritable {
protected:
  Stylesheet* stylesheet;
  bool reference;

public:
  StylesheetStatement() : reference(false){};
  virtual ~StylesheetStatement(){};
  virtual void setStylesheet(Stylesheet* s);
  Stylesheet* getStylesheet();

  void setReference(bool ref);
  bool isReference();

  virtual void process(Stylesheet& s) = 0;
};

#endif  // __less_stylesheet_StylesheetStatement_h__
