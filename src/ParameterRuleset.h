#ifndef __ParameterRuleset_h__
#define __ParameterRuleset_h__

#include "Stylesheet.h"
#include "Selector.h"
#include <map>


class ParameterRuleset: public Ruleset {
public:
  list<string> parameters;
  list<TokenList*> defaults;

  ParameterRuleset(Selector* selector): Ruleset(selector) {
  }

  void addParameter(string keyword, TokenList* value) {
    parameters.push_back(keyword);
    defaults.push_back(value);
  }

  TokenList* getDefault(string keyword) {
    list<string>::iterator pit = parameters.begin();
    list<TokenList*>::iterator dit = defaults.begin();
    for (;pit != parameters.end(); pit++, dit++) {
      if ((*pit) == keyword)
        return (*dit);
    }
    return NULL;
  }

  list<string> getKeywords() {
    return parameters;
  }
};

#endif
