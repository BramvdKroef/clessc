#ifndef __less_lessstylesheet_MediaQueryRuleset_h__
#define __less_lessstylesheet_MediaQueryRuleset_h__

#include <list>
#include <map>

#include "less/stylesheet/Selector.h"
#include "less/stylesheet/Stylesheet.h"

#include "less/Token.h"

#include "less/lessstylesheet/LessRuleset.h"

class MediaQueryRuleset : public LessRuleset {
private:
  TokenList selector;

public:
  MediaQueryRuleset(TokenList &selector,
                    const LessRuleset& parent);
  virtual ~MediaQueryRuleset();

  virtual void process(Stylesheet &s,
                       const Selector *prefix,
                       ProcessingContext &context) const;
};

#endif  // __less_lessstylesheet_MediaQueryRuleset_h__
