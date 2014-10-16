#include "ParameterMixin.h"
#include "LessStylesheet.h"
#include "LessRuleset.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

ParameterMixin::ParameterMixin() {
  this->name = new Selector();
}

ParameterMixin::ParameterMixin(Selector* name) {
  this->name = name;
}


ParameterMixin::~ParameterMixin() {
  map<string, TokenList*>::iterator i;
  
  delete this->name;

  while(!arguments.empty()) {
    delete arguments.back();
    arguments.pop_back();
  }

  for (i = namedArguments.begin(); i != namedArguments.end(); i++) {
    delete i->second;
  }
}

TokenList* ParameterMixin::getArgument(size_t i) {
  if (i < arguments.size())
    return arguments[i];
  else
    return NULL;
}
size_t ParameterMixin::getArgumentCount() {
  return arguments.size();
}
TokenList* ParameterMixin::getArgument(string name) {
  map<string, TokenList*>::iterator i;

  i = namedArguments.find(name);
  
  if (i != namedArguments.end())
    return i->second;
  else
    return NULL;
}


bool ParameterMixin::parse(Selector* selector) {
  
  TokenListIterator* itl = selector->iterator();
  
  while (itl->hasNext() &&
         itl->next()->type != Token::PAREN_OPEN) {
    this->name->push(itl->current()->clone());
  }

  while(this->name->back()->type == Token::WHITESPACE)
    delete this->name->pop();

  parseArguments(itl);
  
  delete itl;
  return true;
}

bool ParameterMixin::insert(Stylesheet* s, Ruleset* ruleset,
                            LessRuleset* parent) {
  vector<TokenList*>::iterator arg_i;
  map<string, TokenList*>::iterator argn_i;
  list<LessRuleset*>::iterator i;
  list<LessRuleset*> rulesetList;
  LessRuleset* lessruleset;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Mixin: \"" << *name->toString() << "\"";
#endif
  
  rulesetList = getLessStylesheet()->getLessRulesets(this);

  if (rulesetList.empty())
    return false;
  
  for (arg_i = arguments.begin(); arg_i != arguments.end(); arg_i++) {
#ifdef WITH_LIBGLOG
    VLOG(3) << "Mixin Arg: " << *(*arg_i)->toString();
#endif
    getLessStylesheet()->getValueProcessor()->processValue(*arg_i);
  }

  for (argn_i = namedArguments.begin(); argn_i !=
         namedArguments.end(); argn_i++) {
#ifdef WITH_LIBGLOG
    VLOG(3) << "Mixin Arg " << argn_i->first << ": " << *argn_i->second->toString();
#endif
    getLessStylesheet()->getValueProcessor()->processValue(argn_i->second);
  }
  

  for (i = rulesetList.begin(); i != rulesetList.end();
       i++) {
    lessruleset = *i;

#ifdef WITH_LIBGLOG
    VLOG(3) << "Mixin: " << *lessruleset->getSelector()->toString();
#endif

    if (parent == NULL || parent != lessruleset ||
        lessruleset->getLessSelector()->needsArguments()) {
      if (ruleset != NULL)
        lessruleset->insert(this, ruleset);
      else
        lessruleset->insert(this, s);
    }
  }

  return !rulesetList.empty();
}

void ParameterMixin::setStylesheet(LessStylesheet* s) {
  lessStylesheet = s;
  stylesheet = s;
}

LessStylesheet* ParameterMixin::getLessStylesheet() {
  return lessStylesheet;
}


void ParameterMixin::process(Stylesheet* s) {
  insert(s, NULL, NULL);
}

void ParameterMixin::parseArguments(TokenListIterator* tli) {
  TokenListIterator tli2 = *tli;
  string delimiter = ",";

  TokenList* argument;
  size_t nestedParenthesis = 0;
  string argName;

  // if a ';' token occurs then that is the delimiter instead of the ','.
  while (tli2.hasNext()) {
    if (tli2.next()->str == ";") {
      delimiter = ";";
      break;
    }
  }

  while (tli->hasNext() && tli->next()->type != Token::PAREN_CLOSED) {
    argument = new TokenList();
    argName = "";

    while (tli->hasNext() && tli->current()->type ==
           Token::WHITESPACE) {
      tli->next();
    }

    if (tli->current()->type == Token::ATKEYWORD &&
        tli->hasNext() &&
        tli->peek()->type == Token::COLON) {
          
      argName = tli->current()->str;
      tli->next();
      if (tli->hasNext())
        tli->next();
    }

    while (nestedParenthesis > 0 ||
           (tli->current()->str != delimiter &&
            tli->current()->type != Token::PAREN_CLOSED)) {
      
      if (tli->current()->type == Token::PAREN_OPEN) 
        nestedParenthesis++;
      
      if (tli->current()->type == Token::PAREN_CLOSED) 
        nestedParenthesis--;
      
      argument->push(tli->current()->clone());

      if (tli->hasNext())
        tli->next();
      else
        break;
    }

    if (argName == "")
      this->arguments.push_back(argument);
    else
      this->namedArguments.insert(pair<string, TokenList*>(argName,argument));
  }
}

