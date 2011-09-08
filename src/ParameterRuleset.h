#ifndef __ParameterRuleset_h__
#define __ParameterRuleset_h__

#include "Stylesheet.h"
#include "TokenList.h"
#include <map>


class ParameterRuleset: public Ruleset {
public:
  map<string, TokenList*> parameters;

  ParameterRuleset(TokenList* selector): Ruleset(selector) {
  }

  void addParameter(string keyword, TokenList* value) {
    parameters[keyword] = value;
  }

  TokenList* getParameter(string keyword) {
    if (parameters.count(keyword))
      return parameters[keyword];
  }
};

#endif
