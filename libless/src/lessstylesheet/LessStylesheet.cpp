#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/LessMediaQuery.h"

LessStylesheet::LessStylesheet() {
}

LessStylesheet::~LessStylesheet() {
}

LessRuleset* LessStylesheet::createLessRuleset(LessSelector &selector) {
  std::list<TokenList>::iterator it;
  LessRuleset* r = new LessRuleset(selector, *this);

  addRuleset(*r);
  for(it = selector.getSelectors().begin();
      it != selector.getSelectors().end();
      it++) {
    lessrulesets.insert(std::pair<TokenList, LessRuleset*>((*it), r));
  }
  return r;
}

Mixin* LessStylesheet::createMixin(const TokenList &selector) {
  Mixin* m = new Mixin(selector, *this);

  addStatement(*m);
  return m;
}

LessAtRule* LessStylesheet::createLessAtRule(const Token& keyword) {
  LessAtRule* atrule = new LessAtRule(keyword);
  addAtRule(*atrule);
  return atrule;
}

LessMediaQuery* LessStylesheet::createLessMediaQuery(const TokenList &selector) {
  LessMediaQuery* q = new LessMediaQuery(selector, *this);

  addStatement(*q);
  return q;
}

void LessStylesheet::deleteLessRuleset(LessRuleset& ruleset) {
  std::list<TokenList>::const_iterator it;
  for(it = ruleset.getLessSelector().getSelectors().begin();
      it != ruleset.getLessSelector().getSelectors().end();
      it++) {
    lessrulesets.erase(*it);
  }

  deleteStatement(ruleset);
}

void LessStylesheet::deleteMixin(Mixin& mixin) {
  deleteStatement(mixin);
}

void LessStylesheet::getFunctions(std::list<const Function*>& functionList,
                                  const Mixin& mixin,
                                  const ProcessingContext &context) const {
  std::multimap<TokenList,LessRuleset*>::const_iterator i, low, up;
  const std::list<Closure*>* closures;
  std::list<Closure*>::const_iterator c_it;
  TokenList::const_iterator t_it;
  TokenList search;
  for(t_it = mixin.name.begin();
      t_it != mixin.name.end() &&
        (*t_it).type != Token::WHITESPACE &&
        (*t_it) != ">";
      t_it++) {
    search.push_back(*t_it);
  }

  low = lessrulesets.lower_bound(search);
  up = lessrulesets.upper_bound(search);

  for (i = low; i != up; i++) {
    (*i).second->getFunctions(functionList, mixin, mixin.name.begin(), context);
  }
  
  closures = context.getBaseClosures();
  for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
    (*c_it)->getFunctions(functionList, mixin, mixin.name.begin(), context);
  }
}

void LessStylesheet::putVariable(const std::string& key,
                                 const TokenList& value) {
  variables[key] = value;
}
const TokenList* LessStylesheet::getVariable(const std::string& key) const {
  return variables.getVariable(key);
}
const TokenList* LessStylesheet::getVariable(const std::string& key,
                                             const ProcessingContext &context) const {
  const TokenList* t;

  if ((t = getVariable(key)) != NULL)
    return t;

  return context.getBaseVariable(key);
}

void LessStylesheet::process(Stylesheet& s, void* context) const {
  std::list<Extension>* extensions;

  std::list<Ruleset*>::const_iterator r_it;
  std::list<Extension>::iterator e_it;
  std::list<Closure*> closureScope;
  Selector* selector;

  ((ProcessingContext*)context)->setLessStylesheet(*this);
  Stylesheet::process(s, context);

  // post processing
  extensions = &((ProcessingContext*)context)->getExtensions();

  for (e_it = extensions->begin(); e_it != extensions->end(); e_it++) {
    for (r_it = s.getRulesets().begin(); r_it != s.getRulesets().end(); r_it++) {
      if ((*e_it).isAll()) {
        // go through all rulesets and look for occurances of the extension target.
        (*e_it).replaceInSelector((*r_it)->getSelector());

      } else if ((*r_it)->getSelector().match((*e_it).getTarget())) {
        // look for a ruleset that matches the extension target
        (*r_it)->getSelector().appendSelector((*e_it).getExtension());
      }
    }
  }
}

