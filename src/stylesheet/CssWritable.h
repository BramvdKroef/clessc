
#ifndef __CssWritable_h__
#define __CssWritable_h__

#include "../css/CssWriter.h"

class CssWritable {
public:
  virtual void write(CssWriter &css) = 0; 
};

#endif
