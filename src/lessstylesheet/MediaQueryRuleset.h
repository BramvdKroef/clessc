
#ifndef __MediaQueryRuleset_h__
#define __MediaQueryRuleset_h__

#include "../stylesheet/Stylesheet.h"
#include "../stylesheet/Selector.h"

#include "../Token.h"

#include "LessRuleset.h"

#include <map>
#include <list>


class MediaQueryRuleset: public LessRuleset {
private:
  static const Token BUILTIN_AND;
  
public:
  MediaQueryRuleset();
  virtual ~MediaQueryRuleset();


  virtual void process(Stylesheet &s, Selector* prefix,
                       ProcessingContext &context);
};

#endif
