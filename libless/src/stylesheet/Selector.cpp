#include "less/stylesheet/Selector.h"
#include <iostream>

Selector::Selector(const TokenList &tokens) : TokenList(tokens) {
}

Selector::~Selector() {
}

void Selector::addPrefix(const Selector &prefix) {
  const_iterator start;
  const_iterator end;

  const_iterator prefix_start;
  const_iterator prefix_end;

  TokenList tmp;
  TokenList::iterator tmp_it;
  
  bool containsAmp;

  start = begin();
  
  while (start != this->end()) {
    end = findComma(start);
    
    tmp.clear();
    tmp.splice(tmp.begin(), *this, start, end);
    containsAmp = tmp.contains(Token::OTHER, "&");
    erase(start, end);
    
    prefix_start = prefix.begin();
    prefix_end = prefix.findComma(prefix_start);
    
    do {
      if (containsAmp) {
        for (tmp_it = tmp.begin(); tmp_it != tmp.end(); tmp_it++) {
          if (*tmp_it == "&")
            insert(end, prefix_start, prefix_end);
          else
            insert(end, *tmp_it);
        }
      } else {
        insert(end, prefix_start, prefix_end);
        insert(end, Token::BUILTIN_SPACE);
        insert(end, tmp.begin(), tmp.end());
      }
      
      if (prefix_start != prefix.begin()) 
        insert(end, Token::BUILTIN_COMMA);
      else
        end++;
      
      prefix_start = prefix_end;
      prefix_end = prefix.findComma(prefix_start);
      
    } while(prefix_start != prefix.end());

    start = end;

  }
}

void Selector::appendSelector(const TokenList &selector) {
  if (!empty()) 
    push_back(Token::BUILTIN_COMMA);
  
  insert(end(), selector.begin(), selector.end());
}

Selector::const_iterator Selector::findComma(Selector::const_iterator offset) const {
  return findComma(offset, end());
}

Selector::const_iterator Selector::findComma(Selector::const_iterator offset,
                                             const Selector::const_iterator &limit) const {
  unsigned int parentheses = 0;

  for (; offset != limit; offset++) {
    if (parentheses == 0 && (*offset) == Token::BUILTIN_COMMA) {
      return offset;

    } else {
      if (*offset == Token::BUILTIN_PAREN_OPEN)
        parentheses++;
      else if (*offset == Token::BUILTIN_PAREN_CLOSED)
        parentheses--;
    }
  }
  return offset;
}


const TokenList::const_iterator Selector::walk(const TokenList::const_iterator &t_begin,
                                               const TokenList::const_iterator &t_end) const {
  Selector::const_iterator end, it, t_it;
  it = begin();
  
  while (it != this->end()) {
    end = findComma(it);

    t_it = t_begin;

    walk(t_it, t_end, it, end);

    if (it == end)
      return t_it;
    
    it = end;
  }
  return t_begin;
}

void Selector::walk(TokenList::const_iterator &it1,
                    const TokenList::const_iterator &it1_end,
                    TokenList::const_iterator &it2,
                    const TokenList::const_iterator &it2_end) const {

  while (it1 != it1_end && it2 != it2_end) {
    if (it1 != it1_end && *it1 == ">") {
      it1++;
      while (it1 != it1_end && (*it1).type == Token::WHITESPACE)
        it1++;
    }

    if (it2 != it2_end && *it2 == ">") {
      it2++;
      while (it2 != it2_end && (*it2).type == Token::WHITESPACE)
        it2++;
    }

    if (*it1 != *it2)
        return;

    it1++;
    it2++;
  }
}

bool Selector::match(const TokenList &tokens) const {
  return walk(tokens.begin(), tokens.end()) == tokens.end();
}

int Selector::compare(const TokenList &tokens,
                      const_iterator offset,
                      const const_iterator end) const {
  const_iterator it = tokens.begin();

  walk(it, tokens.end(), offset, end);
  
  if (it == tokens.end())
    return 1;
  if (offset == end)
    return -1;

  return (*it < *offset) ? 1 : -1;
}

TokenList::const_iterator Selector::find(
    const TokenList &list,
    TokenList::const_iterator offset,
    const TokenList::const_iterator limit) const {
  TokenList::const_iterator it;
  
  for (; offset != limit; offset++) {
    it = list.begin();
    walk(it, list.end(), offset, limit);
    if (it == list.end())
      return it;
  }
  return limit;
}
