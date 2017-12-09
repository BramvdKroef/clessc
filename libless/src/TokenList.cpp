
#include "TokenList.h"

TokenList::~TokenList() {
}

void TokenList::ltrim() {
  while (!empty() &&
         front().type == Token::WHITESPACE) {
    pop_front();
  }
}
void TokenList::rtrim() {
  while (!empty() &&
         back().type == Token::WHITESPACE) {
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
  std::list<Token>::const_iterator it;

  for (it = begin(); it != end(); it++) {
    if (*it == t)
      return true;
  }
  return false;
}

bool TokenList::contains(Token::Type type, const std::string &str)
  const {
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
