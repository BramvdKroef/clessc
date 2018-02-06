#ifndef __less_less_LessSelectorParser_h__
#define __less_less_LessSelectorParser_h__

#include "less/Token.h"
#include "less/TokenList.h"
#include "less/lessstylesheet/LessSelector.h"


class LessSelectorParser {
public:
  bool parse(TokenList& tokens,
             LessSelector& selector);

protected:
  bool parseExtension(TokenList &tokens,
                      TokenList::iterator offset,
                      TokenList::iterator begin,
                      LessSelector &s);
  bool isArguments(TokenList &selector,
                   TokenList::iterator it,
                   std::string &delimiter);
  bool parseArguments(TokenList &selector,
                      TokenList::iterator offset,
                      LessSelector &s);
  bool parseParameter(TokenList &selector,
                      TokenList::iterator &it,
                      Token &keyword,
                      TokenList &value,
                      const std::string &delimiter);
  bool parseDefaultValue(TokenList &arguments,
                         TokenList::iterator& it,
                         const std::string &delimiter,
                         TokenList &value);
  bool parseConditions(TokenList &selector,
                       TokenList::iterator it);


};
  
#endif  // __less_less_LessSelectorParser_h__
