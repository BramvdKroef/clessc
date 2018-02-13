#include "less/TokenList.h"

TokenList::~TokenList() {
}

void TokenList::ltrim() {
  while (!empty() && front().type == Token::WHITESPACE) {
    pop_front();
  }
}
void TokenList::rtrim() {
  while (!empty() && back().type == Token::WHITESPACE) {
    pop_back();
  }
}
void TokenList::trim() {
  ltrim();
  rtrim();
}

std::string TokenList::toString() const {
  std::string str;
  std::list<Token>::const_iterator it;

  for (it = begin(); it != end(); it++) {
    str.append(*it);
  }
  return str;
}

bool TokenList::contains(const Token &t) const {
  return (find(t, begin()) != end());
}

bool TokenList::contains(Token::Type type, const std::string &str) const {
  std::list<Token>::const_iterator it;

  for (it = begin(); it != end(); it++) {
    if ((*it).type == type && *it == str)
      return true;
  }
  return false;
}

bool TokenList::containsType(Token::Type type) const {
  std::list<Token>::const_iterator it;

  for (it = begin(); it != end(); it++) {
    if ((*it).type == type)
      return true;
  }
  return false;
}

TokenList::const_iterator TokenList::find(const Token &search,
                                    TokenList::const_iterator offset) const {
  for (; offset != end(); offset++) {
    if (*offset == search)
      return offset;
  }
  return end();
}

TokenList::const_iterator TokenList::find(const TokenList &search,
                                          TokenList::const_iterator &offset) const {
  TokenList::const_iterator it, it2;
  
  while ((it = find(search.front(), offset)) != end()) {
    offset = it;
    for (it2 = search.begin();
         it != end() && it2 != search.end() && *it == *it2;
         it++, it2++) {
    }
    if (it2 == search.end())
      return it;
    else
      offset++;
  }
  return begin();
}
