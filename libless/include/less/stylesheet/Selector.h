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
class Selector: public std::list<TokenList> {
protected:
  
public:
  Selector();
  virtual ~Selector();

  void appendSelector(const Selector &selector);
  
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
