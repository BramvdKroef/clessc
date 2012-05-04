#ifndef __CssWriter_h__
#define __CssWriter_h__

#include "Stylesheet.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class CssWriter {
public:
  CssWriter(ostream* out);
  void writeStylesheet(Stylesheet* s);

protected:
  ostream* out;
  void writeAtRule(AtRule* rule);
  void writeRuleset(Ruleset* ruleset);
  void writeDeclaration(Declaration* declaration);
  
};
  
#endif
