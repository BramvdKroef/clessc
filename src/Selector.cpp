#include "Selector.h"
#include <iostream>

Selector::~Selector() {
  clear();
}

void Selector::addPrefix(Selector* prefix) {
  list<TokenList*>* prefixParts = prefix->split();
  list<TokenList*>* sepParts = split();
  list<TokenList*>::iterator prefixIt;
  list<TokenList*>::iterator sepIt;
  TokenList* tmp;
  clear();

  for (sepIt = sepParts->begin(); sepIt !=
         sepParts->end(); sepIt++) {

    while ((*sepIt)->front()->type == Token::WHITESPACE)
      delete (*sepIt)->shift();

    if ((*sepIt)->front()->str == "&") 
      delete (*sepIt)->shift();
    else 
      (*sepIt)->unshift(new Token(" ", Token::WHITESPACE));
  }

  for (prefixIt = prefixParts->begin(); prefixIt !=
         prefixParts->end(); prefixIt++) {
    
    for (sepIt = sepParts->begin(); sepIt !=
           sepParts->end(); sepIt++) {
      push(*prefixIt);
      push(*sepIt);
      push(new Token(",", Token::OTHER));
    }
  }
  delete pop();
  
  while (prefixParts->size() > 0) {
    tmp = prefixParts->back();
    prefixParts->pop_back();
    delete tmp;
  }
  while (sepParts->size() > 0) {
    tmp = sepParts->back();
    sepParts->pop_back();
    delete tmp;
  }
}

list<TokenList*>* Selector::split() {
  list<TokenList*>* l = new list<TokenList*>();
  TokenListIterator* it = iterator();
  TokenList* current = new TokenList();
  Token* t;
  
  l->push_back(current);

  while (it->hasNext()) {
    t = it->next();
    if (t->type == Token::OTHER && t->str == ",") {
      current = new TokenList();
      l->push_back(current);
    } else
      current->push(t->clone());
  }
  
  return l;
}
