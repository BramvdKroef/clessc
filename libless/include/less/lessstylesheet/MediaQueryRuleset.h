#ifndef __less_lessstylesheet_MediaQueryRuleset_h__
#define __less_lessstylesheet_MediaQueryRuleset_h__

#include <map>
#include <list>

#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/Selector.h"

#include "less/Token.h"

#include "less/lessstylesheet/LessRuleset.h"

class MediaQueryRuleset: public LessRuleset {
private:
  static const Token BUILTIN_AND;
  
public:
  MediaQueryRuleset();
  virtual ~MediaQueryRuleset();


  virtual void process(Stylesheet &s, Selector* prefix,
                       ProcessingContext &context);
};

#endif // __less_lessstylesheet_MediaQueryRuleset_h__
