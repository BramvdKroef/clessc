#include "ParameterMixin.h"
#include "LessStylesheet.h"
#include "ParameterRuleset.h"

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

bool ParameterMixin::insert(Stylesheet* s, Ruleset* ruleset) {
  Ruleset* mixinRuleset;
  LessRuleset* lessMixinRuleset;

  list<TokenList*>::iterator arg_i;
  list<ParameterRuleset*>::iterator pri;
  list<ParameterRuleset*> prulesetList;
  ParameterRuleset* pruleset;

  bool ret = false;

  DLOG(INFO) << "Mixin: \"" << *name->toString() << "\"";

  if (ruleset != NULL) {
    if ((mixinRuleset = getStylesheet()->getRuleset(name)) != NULL) {
      DLOG(INFO) << "Mixin Ruleset: " << *mixinRuleset->getSelector()->toString();
      mixinRuleset->insert(ruleset);
      ret = true;
    }

    // the mixin is not in a ruleset then only insert nested rules
  } else if ((lessMixinRuleset = getLessStylesheet()->getLessRuleset(name)) != NULL) {
    DLOG(INFO) << "Mixin Ruleset: " << *lessMixinRuleset->getSelector()->toString();
    lessMixinRuleset->insert(s);
    ret = true;
  }

  for (arg_i = arguments->begin(); arg_i != arguments->end(); arg_i++) {
    DLOG(INFO) << "Mixin Arg: " << *(*arg_i)->toString();
    getLessStylesheet()->getValueProcessor()->processValue(*arg_i);
  }
  
  prulesetList = getLessStylesheet()->getParameterRulesets(this);
  
  for (pri = prulesetList.begin(); pri != prulesetList.end();
       pri++) {
    pruleset = *pri;
    
    DLOG(INFO) << "Mixin: " << *pruleset->getSelector()->toString();

    ret = pruleset->insert(arguments, ruleset, s) || ret;
  }

  return ret;
}

void ParameterMixin::setStylesheet(LessStylesheet* s) {
  lessStylesheet = s;
  stylesheet = s;
}

LessStylesheet* ParameterMixin::getLessStylesheet() {
  return lessStylesheet;
}


void ParameterMixin::process(Stylesheet* s) {
  insert(s, NULL);
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

