#ifndef __LessTokenizer_h__
#define __LessTokenizer_h__

#include "CssTokenizer.h"
#include <iostream>

/**
 * Extends CssTokenizer:
 *  - Add support for c++ style comments
 *  
 */
class LessTokenizer: public CssTokenizer {
public:
  LessTokenizer(istream* in) : CssTokenizer(in) {};
protected:
  bool readComment();
};
  
#endif
