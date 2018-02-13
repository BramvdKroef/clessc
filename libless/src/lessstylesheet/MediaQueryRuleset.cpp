#include "less/lessstylesheet/MediaQueryRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"
#include "less/stylesheet/MediaQuery.h"

MediaQueryRuleset::MediaQueryRuleset(TokenList &selector,
                                     const LessRuleset &parent) 
  : LessRuleset(*(new LessSelector()), parent) {
  this->selector = selector;
}
MediaQueryRuleset::~MediaQueryRuleset() {
}

void MediaQueryRuleset::process(Stylesheet& s,
                                const Selector* prefix,
                                ProcessingContext& context) const {
  Selector *rulesetselector;
  TokenList queryselector;
  MediaQuery* query;
  Ruleset* target;

  queryselector = selector;
  context.processValue(queryselector);
    
  query = s.createMediaQuery(queryselector);
  
  if (prefix != NULL) {
    rulesetselector = new Selector(*prefix);
    context.interpolate(*rulesetselector);
    target = query->createRuleset(*rulesetselector);

    processStatements(*target, &context);
  } else
    processStatements(*query, &context);
}
