#include "less/css/CssSelectorParser.h"

bool CssSelectorParser::parse(TokenList& tokens,
                              Selector& selector) {
  TokenList::const_iterator begin, end;
  TokenList tmp;
  TokenList *newSelector;

  begin = tokens.begin();
  
  while (begin != tokens.end()) {
    end = findComma(tokens, begin);

    selector.push_back(tmp);
    newSelector = &selector.back();
    newSelector->assign(begin, end);
    newSelector->trim();
    
    begin = end;
    if (begin != tokens.end())
      begin++;
  }
  return true;
}

TokenList::const_iterator CssSelectorParser::findComma(TokenList &tokens,
                                                       TokenList::const_iterator offset) const {

  unsigned int parentheses = 0;

  for (; offset != tokens.end(); offset++) {
    if (parentheses == 0 && (*offset) == Token::BUILTIN_COMMA) {
      return offset;

    } else {
      if (*offset == Token::BUILTIN_PAREN_OPEN)
        parentheses++;
      else if (*offset == Token::BUILTIN_PAREN_CLOSED)
        parentheses--;
    }
  }
  return offset;
}
