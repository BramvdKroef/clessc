#ifndef __less_less_LessSelectorParser_h__
#define __less_less_LessSelectorParser_h__

#include "less/Token.h"
#include "less/TokenList.h"
#include "less/lessstylesheet/LessSelector.h"
#include "less/css/CssSelectorParser.h"


class LessSelectorParser : public CssSelectorParser {
public:
  bool parse(TokenList& tokens,
             LessSelector& selector);

protected:
  CssSelectorParser selectorParser;
  
  bool parseExtension(TokenList &tokens,
                      TokenList::iterator &offset,
                      LessSelector &s);

  bool isArguments(TokenList &selector,
                   TokenList::iterator it,
                   std::string &delimiter);

  bool parseArguments(TokenList &selector,
                      TokenList::iterator &offset,
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
                       TokenList::iterator &offset,
                       LessSelector &s);


};
  
#endif  // __less_less_LessSelectorParser_h__
