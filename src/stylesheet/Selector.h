
#ifndef __Selector_h__
#define __Selector_h__

#include "TokenList.h"
#include <list>

/**
 * 
 */
class Selector: public TokenList {
public:
  virtual ~Selector();

  void addPrefix(const Selector &prefix);

  /**
   * If the selector contains commas then it is split up into multiple
   * selectors.
   * 
   * For example <code>p .class, a:hover</code> is split up into
   * <code>p .class</code> and <code>a:hover</code>.
   */
  void split(std::list<Selector> &l) const;

  bool match(const Selector &list) const;

  const_iterator walk(const Selector &list, const_iterator offset) const;
  const_iterator walk(const const_iterator &list_begin,
                      const const_iterator &list_end,
                      const_iterator offset) const;

  const_iterator find(const TokenList &list,
                      const_iterator offset,
                      const_iterator limit) const;

  const_iterator findComma(const_iterator offset) const;
  const_iterator findComma(const_iterator offset, const_iterator limit) const;
};

#endif
