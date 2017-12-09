
#ifndef __StylesheetStatement_h__
#define __StylesheetStatement_h__

#include "CssWritable.h"
#include "Stylesheet.h"

class StylesheetStatement : public CssWritable  {
protected:
  Stylesheet* stylesheet;
  bool reference;
  
public:
  StylesheetStatement(): reference(false) {};
  virtual ~StylesheetStatement() {};
  virtual void setStylesheet(Stylesheet* s);
  Stylesheet* getStylesheet();

  void setReference(bool ref);
  bool isReference();
  
  virtual void process(Stylesheet &s) = 0;
};

#endif
