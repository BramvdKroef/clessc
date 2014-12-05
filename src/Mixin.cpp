#include "Mixin.h"
#include "LessStylesheet.h"
#include "LessRuleset.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

Mixin::Mixin() {
}

Mixin::Mixin(const Selector &name) {
  this->name = name;
}


Mixin::~Mixin() {
}

const TokenList* Mixin::getArgument(const size_t i) const {
  if (i < arguments.size())
    return &arguments[i];
  else
    return NULL;
}
size_t Mixin::getArgumentCount() const {
  return arguments.size();
}
const TokenList* Mixin::getArgument(const string &name) const{
  map<string, TokenList>::const_iterator i;

  i = namedArguments.find(name);
  
  if (i != namedArguments.end())
    return &i->second;
  else
    return NULL;
}


bool Mixin::parse(const Selector &selector) {
  
  TokenList::const_iterator i = selector.begin();
  
  for (; i != selector.end() &&
         (*i).type != Token::PAREN_OPEN; i++) {
    this->name.push_back(*i);
  }

  this->name.rtrim();

  parseArguments(i, selector);

  return true;
}

bool Mixin::insert(Stylesheet &s, ProcessingContext &context,
                   Ruleset* target, LessRuleset* parent) {

  vector<TokenList>::iterator arg_i;
  map<string, TokenList>::iterator argn_i;
  list<LessRuleset*>::iterator i;
  list<LessRuleset*> rulesetList;
  LessRuleset* lessruleset;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Mixin: \"" << name.toString() << "\"";
#endif

  if (parent != NULL)
    parent->getLocalLessRulesets(rulesetList, *this);
  else
    getLessStylesheet()->getLessRulesets(rulesetList, *this);

  if (rulesetList.empty())
    return false;
  
  for (arg_i = arguments.begin(); arg_i != arguments.end(); arg_i++) {
#ifdef WITH_LIBGLOG
    VLOG(3) << "Mixin Arg: " << (*arg_i).toString();
#endif
    context.processValue(*arg_i);
  }

  for (argn_i = namedArguments.begin(); argn_i !=
         namedArguments.end(); argn_i++) {
#ifdef WITH_LIBGLOG
    VLOG(3) << "Mixin Arg " << argn_i->first << ": " << argn_i->second.toString();
#endif
    context.processValue(argn_i->second);
  }
  

  for (i = rulesetList.begin(); i != rulesetList.end(); i++) {
    lessruleset = *i;

#ifdef WITH_LIBGLOG
    VLOG(3) << "Mixin: " << lessruleset->getSelector().toString();
#endif

    if (lessruleset->getLessSelector()->needsArguments() ||
        !context.isInStack(*lessruleset)) {
      if (target != NULL)
        lessruleset->insert(this, *target, context);
      else
        lessruleset->insert(this, s, context);
    }
  }

  return !rulesetList.empty();
}

void Mixin::setLessStylesheet(LessStylesheet &s) {
  lessStylesheet = &s;
  stylesheet = &s;
}

LessStylesheet* Mixin::getLessStylesheet() {
  return lessStylesheet;
}


void Mixin::process(Stylesheet &s) {
  insert(s, *getLessStylesheet()->getContext(), NULL, NULL);
}

void Mixin::parseArguments(TokenList::const_iterator i, const Selector &selector) {
  TokenList::const_iterator j;
  string delimiter = ",";

  TokenList argument;
  size_t nestedParenthesis = 0;
  string argName;

  if (i != selector.end() &&
      (*i).type == Token::PAREN_OPEN) {
    i++;
  }
    
  // if a ';' token occurs then that is the delimiter instead of the ','.
  for(j = i; j != selector.end(); j++) {
    if (*j == ";") {
      delimiter = ";";
      break;
    }
  }

  while (i != selector.end() && (*i).type != Token::PAREN_CLOSED) {
    while (i != selector.end() &&
           (*i).type == Token::WHITESPACE) {
      i++;
    }

    if ((*i).type == Token::ATKEYWORD) {
      argName = (*i);
      i++;
      if (i != selector.end() &&
          (*i).type == Token::COLON) {
        i++;
      } else {
        argName = "";
        i--;
      }
    }

    while (i != selector.end() &&
           (nestedParenthesis > 0 ||
            ((*i) != delimiter &&
             (*i).type != Token::PAREN_CLOSED))) {
      
      if ((*i).type == Token::PAREN_OPEN)
        nestedParenthesis++;
      
      if ((*i).type == Token::PAREN_CLOSED)
        nestedParenthesis--;
      
      argument.push_back(*i);
      
      i++;
    }

    if (*i == delimiter)
      i++;

    if (argName == "")
      this->arguments.push_back(argument);
    else {
      this->namedArguments.insert(pair<string,
                                  TokenList>(argName,argument));
      argName = "";
    }
    argument.clear();
  }
}

