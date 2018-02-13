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
class Selector {
protected:
  std::list<TokenList> selectors;

public:
  Selector();
  virtual ~Selector();

  std::list<TokenList> &getSelectors();
  const std::list<TokenList> &getSelectors() const;

  TokenList &appendSelector(const TokenList &selector);
  void appendSelector(const Selector &selector);
  TokenList &insertSelector(const std::list<TokenList>::const_iterator &pos,
                            const TokenList &selector);

  void removeSelector(const std::list<TokenList>::iterator &pos);
 
  const TokenList::const_iterator walk(const TokenList::const_iterator &t_begin,
                                       const TokenList::const_iterator &t_end) const;

  void walk(TokenList::const_iterator &it1,
            const TokenList::const_iterator &it1_end,
            TokenList::const_iterator &it2,
            const TokenList::const_iterator &it2_end) const;

  bool match(const TokenList &tokens) const;
  bool match(const Selector &selector) const;
  int compare(const TokenList &tokens,
              TokenList::const_iterator offset,
              const TokenList::const_iterator end) const;
  bool replace(const TokenList &search,
               const TokenList &replace);

  void addPrefix(const Selector &prefix);

  std::string toString() const;
};

#endif  // __less_stylesheet_Selector_h__
