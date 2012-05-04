#include "CssPrettyWriter.h"


void CssPrettyWriter::writeAtRule(AtRule* atrule) {
  CssWriter::writeAtRule(atrule);
  out->write("\n", 1);
}

void CssPrettyWriter::writeRuleset(Ruleset* ruleset) {
  TokenList* selector = ruleset->getSelector();
  TokenListIterator* it;
  
  vector<Declaration*>* declarations = ruleset->getDeclarations();
  vector<Declaration*>::iterator dIt;

  if (declarations->size() == 0)
    return;
  
  if (selector != NULL) {
    for (it = selector->iterator(); it->hasNext();) {
      Token* next = it->next();
      out->write(next->str.c_str(), next->str.size());
    }
  }
  out->write("{\n", 2);
  for (dIt = declarations->begin(); dIt < declarations->end(); dIt++) {
    out->write("    ", 4);
    writeDeclaration(*dIt);
    out->write(";\n", 2);
  }
  out->write("}\n", 2);
}
