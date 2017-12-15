
#include "Stylesheet.h"

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif


Stylesheet::~Stylesheet() {
  rulesets.clear();
  atrules.clear();
  while(!statements.empty()) {
    delete statements.back();
    statements.pop_back();
  }
}

void Stylesheet::addStatement(StylesheetStatement &statement) {
  statements.push_back(&statement);
  statement.setStylesheet(this);
  
#ifdef WITH_LIBGLOG
  VLOG(3) << "Adding statement";
#endif
}
void Stylesheet::addRuleset(Ruleset &ruleset) {
  addStatement(ruleset);
  rulesets.push_back(&ruleset);
}
void Stylesheet::addAtRule(AtRule &rule) {
  addStatement(rule);
  atrules.push_back(&rule);
}


Ruleset* Stylesheet::createRuleset() {
  Ruleset* r = new Ruleset();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating ruleset";
#endif

  addRuleset(*r);
  
  return r;
}

Ruleset* Stylesheet::createRuleset(const Selector &selector) {
  Ruleset* r = new Ruleset(selector);

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating ruleset: " << selector.toString();
#endif

  addRuleset(*r);
  
  return r;
}

AtRule* Stylesheet::createAtRule(const Token &keyword) {
  AtRule* r = new AtRule(keyword);

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating @rule";
#endif
  
  addStatement(*r);
  atrules.push_back(r);
  return r;
}

CssComment* Stylesheet::createComment() {
  CssComment* c = new CssComment();
  addStatement(*c);
  return c;
}

MediaQuery* Stylesheet::createMediaQuery() {
  MediaQuery* q = new MediaQuery();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating media query";
#endif
  
  addStatement(*q);
  return q;
}

void Stylesheet::deleteStatement(StylesheetStatement &statement) {
  statements.remove(&statement);
  delete &statement;
}

void Stylesheet::deleteRuleset(Ruleset &ruleset) {
  rulesets.remove(&ruleset);
  deleteStatement(ruleset);
}
void Stylesheet::deleteAtRule(AtRule &atrule) {
  atrules.remove(&atrule);
  deleteStatement(atrule);
}
void Stylesheet::deleteMediaQuery(MediaQuery &query) {
  deleteStatement(query);
}

std::list<AtRule*>& Stylesheet::getAtRules() {
  return atrules;
}
std::list<Ruleset*>& Stylesheet::getRulesets() {
  return rulesets;
}
std::list<StylesheetStatement*>& Stylesheet::getStatements() {
  return statements;
}

Ruleset* Stylesheet::getRuleset(const Selector &selector) {
  std::list<Ruleset*>::iterator it;

  for (it = rulesets.begin(); it != rulesets.end(); it++) {
    if ((*it)->getSelector().match(selector))
      return *it;
  }
  return NULL;
}

void Stylesheet::process(Stylesheet &s) {
  std::list<StylesheetStatement*> statements = getStatements();
  std::list<StylesheetStatement*>::iterator i;
  
#ifdef WITH_LIBGLOG
  VLOG(1) << "Processing stylesheet";
#endif
        
  for (i = statements.begin(); i != statements.end(); i++) {
    if ((*i)->isReference() == false)
      (*i)->process(s);
  }
  
#ifdef WITH_LIBGLOG
  VLOG(1) << "Done processing stylesheet";
#endif
}


void Stylesheet::write(CssWriter &writer) {
  std::list<StylesheetStatement*> statements = getStatements();
  std::list<StylesheetStatement*>::iterator i;
  
  for (i = statements.begin(); i != statements.end(); i++) {
    (*i)->write(writer);
  }
}

