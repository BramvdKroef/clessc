#ifndef __TokenList_h__
#define __TokenList_h__

#include "Token.h"
#include <list>
using namespace std;

class TokenListIterator {
private:
  list<Token*>::iterator it, begin, end;
  
public:
  TokenListIterator(list<Token*>::iterator begin,
                    list<Token*>::iterator end) {
    this->it = begin;
    this->begin = begin;
    this->end = --end;
  }
  void toEnd () {
    it = end;
  }
  Token* previous () {
    return *--it;
  }
  Token* next () {
    return *++it;
  }
  Token* peek () {
    return *it;
  }
  bool hasNext () {
    return (it != end);
  }
  bool hasPrevious () {
    return (it != begin);
  }
};

class TokenList {
private:
  list<Token*> tokens;
  
public:
  ~TokenList() {
    while (!empty()) 
      delete pop();
  }
  
  void push (Token* token) {
    tokens.push_back(token);
  }
  Token* pop () {
    Token* token = tokens.back();
    tokens.pop_back();
    return token;
  }

  void unshift (Token* token) {
    tokens.push_front(token);
  }
  
  Token* shift () {
    Token* token = tokens.front();
    tokens.pop_front();
    return token;
  }

  bool empty () {
    return tokens.empty();
  }

  int size() {
    return tokens.size();
  }

  bool equals(TokenList* list) {
    return false;
  }

  Token* back() {
    return tokens.back();
  }

  Token* front() {
    return tokens.front();
  }

  TokenListIterator* iterator() {
    return new TokenListIterator(tokens.begin(), tokens.end());
  }
  TokenListIterator* reverseIterator () {
    TokenListIterator* it =
      new TokenListIterator(tokens.begin(), tokens.end());
    it->toEnd();
    return it;
  }

  void push(TokenList* list) {
    TokenListIterator* it = list->iterator();
    while (it->hasNext())
      push(it->next()->clone());
    delete it;
  }
  void unshift(TokenList* list) {
    TokenListIterator* it = list->reverseIterator();
    while (it->hasNext())
      unshift(it->next()->clone());
    delete it;
  }

  TokenList* clone() {
    TokenList* newtokens = new TokenList();
    list<Token*>::iterator it;
    
    for (it = tokens.begin(); it != tokens.end(); it++) {
      newtokens->push((*it)->clone());
    }
    return newtokens;
  }
  
  string* toString() {
    string* str = new string();
    list<Token*>::iterator it;
    
    for (it = tokens.begin(); it != tokens.end(); it++) {
      str->append((*it)->str);
    }
    return str;
  }
};

#endif
