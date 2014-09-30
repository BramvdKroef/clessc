#include "ParameterMixin.h"

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
  
  size_t nestedParenthesis = 0;

  while (itl->hasNext() &&
         itl->next()->type != Token::PAREN_OPEN) {
    this->name->push(itl->current()->clone());
  }
  
  parseArguments(itl);
  
  delete itl;
  return true;
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
          current->str == delimiter)
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

