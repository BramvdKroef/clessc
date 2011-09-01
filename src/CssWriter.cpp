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
  TokenList* rule = atrule->getRule();
  TokenListIterator* it = rule->iterator();
  
  out->write(atrule->getKeyword()->c_str(),
             atrule->getKeyword()->size());
  out->write(" ", 1);
  
  while (it->hasNext()) {
    Token* next = it->next();
    out->write(next->str.c_str(), next->str.size());
  }

}

void CssWriter::writeRuleset(Ruleset* ruleset) {
  TokenList* selector = ruleset->getSelector();
  TokenListIterator* it;
  
  vector<Declaration*>* declarations = ruleset->getDeclarations();
  vector<Declaration*>::iterator dIt;
  
  if (selector != NULL) {
    for (it = selector->iterator(); it->hasNext();) {
      Token* next = it->next();
      out->write(next->str.c_str(), next->str.size());
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
  TokenList* value = declaration->getValue();
  TokenListIterator* it = value->iterator();
    
  out->write(declaration->getProperty()->c_str(),
             declaration->getProperty()->size());
  out->write(":", 1);
  
  while (it->hasNext()) {
    Token* next = it->next();
    out->write(next->str.c_str(), next->str.size());
  }
}

