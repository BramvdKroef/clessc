#include "ParameterMixin.h"
#include "LessStylesheet.h"
#include "LessRuleset.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

ParameterMixin::ParameterMixin() {
  this->name = new Selector();
  this->arguments = new list<TokenList*>();
}

ParameterMixin::ParameterMixin(Selector* name, list<TokenList*>* arguments) {
  this->name = name;
  this->arguments = arguments;
}


ParameterMixin::~ParameterMixin() {
  delete this->name;

  while(!arguments->empty()) {
    delete arguments->back();
    arguments->pop_back();
  }
  delete arguments;
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
  list<TokenList*>::iterator arg_i;
  list<LessRuleset*>::iterator i;
  list<LessRuleset*> rulesetList;
  LessRuleset* lessruleset;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Mixin: \"" << *name->toString() << "\"";
#endif
  
  for (arg_i = arguments->begin(); arg_i != arguments->end(); arg_i++) {
#ifdef WITH_LIBGLOG
    VLOG(3) << "Mixin Arg: " << *(*arg_i)->toString();
#endif
    
    getLessStylesheet()->getValueProcessor()->processValue(*arg_i);
  }

  rulesetList = getLessStylesheet()->getLessRulesets(this);
  
  for (i = rulesetList.begin(); i != rulesetList.end();
       i++) {
    lessruleset = *i;

#ifdef WITH_LIBGLOG
    VLOG(3) << "Mixin: " << *lessruleset->getSelector()->toString();
#endif

    if (parent == NULL || parent != lessruleset ||
        lessruleset->getLessSelector()->needsArguments()) {
      if (ruleset != NULL)
        lessruleset->insert(arguments, ruleset);
      else
        lessruleset->insert(arguments, s);
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
  Token* current;
  size_t nestedParenthesis = 0;

  // if a ';' token occurs then that is the delimiter instead of the ','.
  while (tli2.hasNext()) {
    if (tli2.next()->str == ";") {
      delimiter = ";";
      break;
    }
  }

  while (tli->hasNext() && tli->peek()->type != Token::PAREN_CLOSED) {
    argument = new TokenList();

    while (tli->hasNext()) {
      current = tli->next();
      if (nestedParenthesis == 0 &&
          (current->str == delimiter ||
           current->type == Token::PAREN_CLOSED))
        break;
      
      if (current->type == Token::PAREN_OPEN) 
        nestedParenthesis++;
      
      if (current->type == Token::PAREN_CLOSED) 
        nestedParenthesis--;
      
      argument->push(current->clone());
    }

    this->arguments->push_back(argument);
  }
}

