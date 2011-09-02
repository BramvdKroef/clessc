#include "TokenList.h"

TokenListIterator::TokenListIterator(list<Token*>::iterator begin,
                  list<Token*>::iterator end) {
  it = begin;
  this->begin = begin;
  this->end = --end;
  it--;
}
void TokenListIterator::toEnd () {
  it = end;
  it++;
}
Token* TokenListIterator::previous () {
  return *--it;
}
Token* TokenListIterator::next () {
  return *++it;
}
Token* TokenListIterator::peek () {
  Token* ret;
  it++;
  ret = *it;
  it--;
  return ret;
}
bool TokenListIterator::hasNext () {
  return (it != end);
}
bool TokenListIterator::hasPrevious () {
  return (it != begin);
}

TokenList::~TokenList() {
  while (!empty()) 
    delete pop();
}
  
void TokenList::push (Token* token) {
  tokens.push_back(token);
}
Token* TokenList::pop () {
  Token* token = tokens.back();
  tokens.pop_back();
  return token;
}

void TokenList::unshift (Token* token) {
  tokens.push_front(token);
}
  
Token* TokenList::shift () {
  Token* token = tokens.front();
  tokens.pop_front();
  return token;
}

bool TokenList::empty () {
  return tokens.empty();
}

int TokenList::size() {
  return tokens.size();
}

bool TokenList::equals(TokenList* list) {
  TokenListIterator* it1, * it2;
  if (list->size() != size())
    return false;
    
  it1 = iterator();
  it2 = list->iterator();
    
  while (it1->hasNext() && it2->hasNext()) {
    if (!it1->next()->equals(it2->next())) {
      delete it1;
      delete it2;
      return false;
    }
  }
  return true;
}

Token* TokenList::back() {
  return tokens.back();
}

Token* TokenList::front() {
  return tokens.front();
}

TokenListIterator* TokenList::iterator() {
  return new TokenListIterator(tokens.begin(), tokens.end());
}
TokenListIterator* TokenList::reverseIterator () {
  TokenListIterator* it =
    new TokenListIterator(tokens.begin(), tokens.end());
  it->toEnd();
  return it;
}

void TokenList::push(TokenList* list) {
  TokenListIterator* it = list->iterator();
  while (it->hasNext())
    push(it->next()->clone());
  delete it;
}
void TokenList::unshift(TokenList* list) {
  TokenListIterator* it = list->reverseIterator();
  while (it->hasPrevious())
    unshift(it->previous()->clone());
  delete it;
}

TokenList* TokenList::clone() {
  TokenList* newtokens = new TokenList();
  list<Token*>::iterator it;
    
  for (it = tokens.begin(); it != tokens.end(); it++) {
    newtokens->push((*it)->clone());
  }
  return newtokens;
}
  
string* TokenList::toString() {
  string* str = new string();
  list<Token*>::iterator it;
    
  for (it = tokens.begin(); it != tokens.end(); it++) {
    str->append((*it)->str);
  }
  return str;
}
