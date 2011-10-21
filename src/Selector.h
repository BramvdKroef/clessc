#ifndef __Selector_h__
#define __Selector_h__

#include "TokenList.h"
#include <list>
using namespace std;

/**
 * 
 */
class Selector: public TokenList {
public:
  virtual ~Selector();

  void addPrefix(Selector* prefix);

  /**
   * If the selector contains commas then it is split up into multiple
   * selectors.
   * 
   * For example <code>p .class, a:hover</code> is split up into
   * <code>p .class</code> and <code>a:hover</code>.
   */
  list<TokenList*>* split();
  
};

#endif
