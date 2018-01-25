#ifndef __less_stylesheet_CssWritable_h__
#define __less_stylesheet_CssWritable_h__

#include "less/css/CssWriter.h"

class CssWritable {
public:
  virtual void write(CssWriter &css) const = 0;
};

#endif  // __less_stylesheet_CssWritable_h__
