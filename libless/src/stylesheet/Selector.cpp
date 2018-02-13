#include "less/stylesheet/Selector.h"


Selector::Selector() {
}

Selector::~Selector() {
}

std::list<TokenList> &Selector::getSelectors() {
  return selectors;
}
const std::list<TokenList> &Selector::getSelectors() const {
  return selectors;
}

TokenList &Selector::appendSelector(const TokenList &selector) {
  selectors.push_back(selector);
  return selectors.back();
}
void Selector::appendSelector(const Selector &selector) {
  std::list<TokenList>::const_iterator it;
  
  for (it = selector.getSelectors().begin();
       it != selector.getSelectors().end();
       it++) {
    
    selectors.push_back(*it);
  }
}

TokenList &Selector::insertSelector(const std::list<TokenList>::const_iterator &pos,
                                    const TokenList &selector) {
  return *(selectors.insert(pos, selector));
}

void Selector::removeSelector(const std::list<TokenList>::iterator &pos) {
  selectors.erase(pos);
}

const TokenList::const_iterator Selector::walk(const TokenList::const_iterator &t_begin,
                                               const TokenList::const_iterator &t_end) const {
  std::list<TokenList>::const_iterator it;
  TokenList::const_iterator t_it1, t_it2;
  
  for (it = selectors.begin(); it != selectors.end(); it++) {
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
  std::list<TokenList>::const_iterator it;
  
  for (it = selector.getSelectors().begin();
       it != selector.getSelectors().end();
       it++) {
    if (match(*it))
      return true;
  }
  return false;
}

bool Selector::replace(const TokenList &find,
                       const TokenList &replace) {
  std::list<TokenList>::const_iterator s_it;
  
  TokenList::const_iterator t_it, t_match, t_start;
  TokenList newselector;
  bool ret = false;

  for (s_it = getSelectors().begin();
       s_it != getSelectors().end();
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
      appendSelector(newselector);
      newselector.clear();
      ret = true;
    }
  }
  return ret;
}

void Selector::addPrefix(const Selector &prefix) {
  std::list<TokenList>::iterator it;
  std::list<TokenList>::const_iterator it2;
  TokenList tl;
  TokenList *inserted;

  const TokenList* tmp;
  TokenList::const_iterator tmp_it;
  
  
  bool containsAmp;

  for (it = selectors.begin(); it != selectors.end(); ) {
    tmp = &(*it);
    containsAmp = tmp->contains(Token::OTHER, "&");
    
    for (it2 = prefix.getSelectors().begin();
         it2 != prefix.getSelectors().end();
         it2++) {
      inserted = &insertSelector(it, tl);
      
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
    it = selectors.erase(it);
  }

}

std::string Selector::toString() const {
  std::string str;
  std::list<TokenList>::const_iterator it;

  for (it = selectors.begin(); it != selectors.end(); it++) {
    if (it != selectors.begin())
      str.append(", ");
    
    str.append((*it).toString());
  }
  return str;
}

