#include "less/stylesheet/Selector.h"


Selector::Selector() {
}

Selector::~Selector() {
}

void Selector::appendSelector(const Selector &selector) {
  insert(end(), selector.begin(), selector.end());
}


const TokenList::const_iterator Selector::walk(const TokenList::const_iterator &t_begin,
                                               const TokenList::const_iterator &t_end) const {
  const_iterator it;
  TokenList::const_iterator t_it1, t_it2;
  
  for (it = begin(); it != end(); it++) {
    t_it1 = (*it).begin();
    t_it2 = t_begin;
    
    walk(t_it2, t_end, t_it1, (*it).end());
    if (t_it1 == (*it).end())
      return t_it2;
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
bool Selector::match(const Selector &selector) const {
  const_iterator it;
  
  for (it = selector.begin();
       it != selector.end();
       it++) {
    if (match(*it))
      return true;
  }
  return false;
}

bool Selector::replace(const TokenList &find,
                       const TokenList &replace) {
  const_iterator s_it;
  
  TokenList::const_iterator t_it, t_match, t_start;
  TokenList newselector;
  bool ret = false;

  for (s_it = begin();
       s_it != end();
       s_it++) {

    t_it = t_start = (*s_it).begin();
    t_match = (*s_it).find(find, t_it);
    
    if (t_match != (*s_it).begin()) {
      newselector.insert(newselector.end(), t_start, t_it);
      newselector.insert(newselector.end(), replace.begin(), replace.end());
      
      t_it = t_start = t_match;
      while ((t_match = (*s_it).find(find, t_it)) != (*s_it).begin()) {
        newselector.insert(newselector.end(), t_start, t_it);
        newselector.insert(newselector.end(), replace.begin(), replace.end());
        t_it = t_start = t_match;
      }
      newselector.insert(newselector.end(), t_start, (*s_it).end());
      push_back(newselector);
      newselector.clear();
      ret = true;
    }
  }
  return ret;
}

void Selector::addPrefix(const Selector &prefix) {
  iterator it;
  const_iterator it2;
  TokenList tl;
  TokenList *inserted;

  const TokenList* tmp;
  TokenList::const_iterator tmp_it;
  
  
  bool containsAmp;

  for (it = begin(); it != end(); ) {
    tmp = &(*it);
    containsAmp = tmp->contains(Token::OTHER, "&");
    
    for (it2 = prefix.begin();
         it2 != prefix.end();
         it2++) {
      inserted = &(*insert(it, tl));
      
      if (containsAmp) {
        for (tmp_it = tmp->begin(); tmp_it != tmp->end(); tmp_it++) {
          if (*tmp_it == "&")
            inserted->insert(inserted->end(), (*it2).begin(), (*it2).end());
          else
            inserted->push_back(*tmp_it);
        }
      } else {
        inserted->insert(inserted->end(), (*it2).begin(), (*it2).end());
        inserted->push_back(Token::BUILTIN_SPACE);
        inserted->insert(inserted->end(), tmp->begin(), tmp->end());
      }
    }
    it = erase(it);
  }

}

std::string Selector::toString() const {
  std::string str;
  const_iterator it;

  for (it = begin(); it != end(); it++) {
    if (it != begin())
      str.append(", ");
    
    str.append((*it).toString());
  }
  return str;
}

