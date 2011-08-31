#include "Writer.h"

Writer::Writer(ostream* out) {
  this->out = out;
}

void Writer::writeStylesheet(Stylesheet* s) {
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

void Writer::writeAtRule(AtRule* atrule) {
  vector<string*>* rule = atrule->getRule();
  vector<string*>::iterator it;
  
  out->write(atrule->getKeyword()->c_str(),
             atrule->getKeyword()->size());
  
  for (it = rule->begin(); it < rule->end(); it++) {
    out->write((*it)->c_str(), (*it)->size());
  }
}

void Writer::writeRuleset(Ruleset* ruleset) {
  vector<string*>* selector = ruleset->getSelector();
  vector<string*>::iterator it;
  
  vector<Declaration*>* declarations = ruleset->getDeclarations();
  vector<Declaration*>::iterator dIt;
  
  if (selector != NULL) {
    for (it = selector->begin(); it < selector->end(); it++) {
      out->write((*it)->c_str(), (*it)->size());
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

void Writer::writeDeclaration(Declaration* declaration) {
  vector<string*>* value = declaration->getValue();
  vector<string*>::iterator it;
  out->write(declaration->getProperty()->c_str(),
             declaration->getProperty()->size());
  out->write(":", 1);
  
  for (it = value->begin(); it < value->end(); it++) {
    out->write((*it)->c_str(), (*it)->size());
  }
}

