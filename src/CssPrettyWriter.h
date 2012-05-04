#ifndef __CssPrettyWriter_h__
#define __CssPrettyWriter_h__

#include "CssWriter.h"

class CssPrettyWriter: public CssWriter {
public:
  CssPrettyWriter(ostream* out): CssWriter(out) {
  };

protected:
  void writeAtRule(AtRule* rule);
  void writeRuleset(Ruleset* ruleset);
};

#endif
