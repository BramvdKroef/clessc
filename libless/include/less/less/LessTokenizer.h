
#ifndef __LessTokenizer_h__
#define __LessTokenizer_h__

#include <less/css/CssTokenizer.h>
#include <iostream>

/**
 * Extends CssTokenizer:
 *  - Add support for c++ style comments
 *  
 */
class LessTokenizer: public CssTokenizer {
public:
  LessTokenizer(istream &in, const char* source) : CssTokenizer(in, source) {};
  virtual ~LessTokenizer();
protected:
  bool readComment();
};
  
#endif
