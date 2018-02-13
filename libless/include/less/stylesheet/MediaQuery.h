#ifndef __less_stylesheet_MediaQuery_h__
#define __less_stylesheet_MediaQuery_h__

#include "less/TokenList.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"

class MediaQuery : public Stylesheet, public StylesheetStatement {
private:
  TokenList selector;
  static const Token BUILTIN_AND;

public:
  MediaQuery(const TokenList &selector);

  TokenList& getSelector();
  const TokenList& getSelector() const;

  virtual MediaQuery *createMediaQuery(const TokenList &selector);

  virtual void process(Stylesheet& s, void* context) const;
  virtual void write(CssWriter& writer) const;
};

#endif  // __less_stylesheet_MediaQuery_h__
