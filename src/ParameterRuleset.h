#ifndef __ParameterRuleset_h__
#define __ParameterRuleset_h__

#include "Stylesheet.h"
#include "Selector.h"
#include "ParseException.h"
#include <map>


class ParameterRuleset: public Ruleset {
protected:
  bool processParameter(Selector* selector);
  
public:
  list<string> parameters;
  list<TokenList*> defaults;

  ParameterRuleset(Selector* selector);

  void addParameter(string keyword, TokenList* value);

  TokenList* getDefault(string keyword);

  list<string> getKeywords();
};

#endif
