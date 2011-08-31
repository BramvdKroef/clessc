#include "CssWriter.h"

CssWriter::CssWriter(ostream* out) {
  this->out = out;
}

void CssWriter::writeStylesheet(Stylesheet* s) {
  vector<AtRule*>* atrules = s->getAtRules();
  vector<AtRule*>::iterator aIt;
  vector<Ruleset*>* rulesets = s->getRulesets();
  vector<Ruleset*>::iterator rIt;

  for (aIt = atrules->begin(); aIt < atrules->end(); aIt++) {
    writeAtRule(*aIt);
    out->write(";", 1);
  }
  for (rIt = rulesets->begin(); rIt < rulesets->end(); rIt++) {
    writeRuleset(*rIt);
  }
}

void CssWriter::writeAtRule(AtRule* atrule) {
  vector<Token*>* rule = atrule->getRule();
  vector<Token*>::iterator it;
  
  out->write(atrule->getKeyword()->c_str(),
             atrule->getKeyword()->size());
  out->write(" ");
  
  for (it = rule->begin(); it < rule->end(); it++) {
    out->write((*it)->str.c_str(), (*it)->str.size());
  }
}

void CssWriter::writeRuleset(Ruleset* ruleset) {
  vector<Token*>* selector = ruleset->getSelector();
  vector<Token*>::iterator it;
  
  vector<Declaration*>* declarations = ruleset->getDeclarations();
  vector<Declaration*>::iterator dIt;
  
  if (selector != NULL) {
    for (it = selector->begin(); it < selector->end(); it++) {
      out->write((*it)->str.c_str(), (*it)->str.size());
    }
  }
  out->write("{", 1);
  for (dIt = declarations->begin(); dIt < declarations->end(); dIt++) {
    writeDeclaration(*dIt);
    if (dIt + 1 < declarations->end())
      out->write(";", 1);
  }
  out->write("}", 1);
}

void CssWriter::writeDeclaration(Declaration* declaration) {
  vector<Token*>* value = declaration->getValue();
  vector<Token*>::iterator it;
  out->write(declaration->getProperty()->c_str(),
             declaration->getProperty()->size());
  out->write(":", 1);
  
  for (it = value->begin(); it < value->end(); it++) {
    out->write((*it)->str.c_str(), (*it)->str.size());
  }
}

