#ifndef __less_TokenList_h__
#define __less_TokenList_h__

#include <list>
#include "less/Token.h"

class TokenList : public std::list<Token> {
public:
  virtual ~TokenList();

  /**
   * Trim whitespace tokens from the front of the selector.
   */
  void ltrim();

  /**
   * Trim whitespace tokens from the back of the selector.
   */
  void rtrim();

  /**
   * Trim whitespace tokens from the front and back of the selector.
   */
  void trim();

  std::string toString() const;

  bool contains(const Token &t) const;
  bool contains(Token::Type t, const std::string &str) const;
  bool containsType(Token::Type t) const;

  const_iterator find(const Token &find,
                      const_iterator offset) const;
  const_iterator find(const TokenList &find,
                      const_iterator &offset) const;
};

#endif  // __less_TokenList_h__
