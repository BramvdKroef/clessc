#ifndef __Writer_h__
#define __Writer_h__

#include "Stylesheet.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Writer {
public:
  Writer(ostream* out);
  void writeStylesheet(Stylesheet* s);

private:
  ostream* out;
  void writeAtRule(AtRule* rule);
  void writeRuleset(Ruleset* ruleset);
  void writeDeclaration(Declaration* declaration);
  
};
  
#endif
