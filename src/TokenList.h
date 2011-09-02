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
                    list<Token*>::iterator end);
  void toEnd ();
  Token* previous ();
  Token* next ();
  Token* peek ();
  Token* current ();
  bool hasNext ();
  bool hasPrevious ();
};

class TokenList {
private:
  list<Token*> tokens;
  
public:
  ~TokenList();
  
  void push (Token* token);
  Token* pop ();

  void unshift (Token* token);
  Token* shift ();

  bool empty ();
  int size();

  bool equals(TokenList* list);

  Token* back();
  Token* front();

  TokenListIterator* iterator();
  TokenListIterator* reverseIterator ();

  void push(TokenList* list);
  void unshift(TokenList* list);

  TokenList* clone();
  
  string* toString();
};

#endif
