#ifndef __less_css_CssSelectorParser_h__
#define __less_css_CssSelectorParser_h__

#include "less/Token.h"
#include "less/TokenList.h"
#include "less/stylesheet/Selector.h"

class CssSelectorParser {
public:
  bool parse(TokenList& tokens,
             Selector& selector);

private:
  TokenList::const_iterator findComma(TokenList &tokens,
                                      TokenList::const_iterator offset) const;

};

#endif  // __less_css_CssSelectorParser_h__
