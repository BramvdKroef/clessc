#ifndef __less_stylesheet_Selector_h__
#define __less_stylesheet_Selector_h__

#include <list>
#include "less/TokenList.h"

/**
 * If the selector contains commas then it is split up into multiple
 * selectors.
 *
 * For example <code>p .class, a:hover</code> is split up into
 * <code>p .class</code> and <code>a:hover</code>.
 */
class Selector : public TokenList {

public:
  Selector();
  Selector(const TokenList &tokens);
  virtual ~Selector();

  void addPrefix(const Selector &prefix);

 
  TokenList::const_iterator walk(TokenList::const_iterator t_begin,
                                 TokenList::const_iterator t_end) const;

  const_iterator find(const TokenList &list,
                                 const_iterator offset,
                                 const_iterator limit) const;

  bool match(const TokenList &tokens) const;
  int compare(const TokenList &tokens, const_iterator offset);
  
  const_iterator findComma(const_iterator offset) const;
  const_iterator findComma(const_iterator offset,
                                      const_iterator limit) const;
};

#endif  // __less_stylesheet_Selector_h__
