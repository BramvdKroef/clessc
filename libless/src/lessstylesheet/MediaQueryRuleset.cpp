#include "less/lessstylesheet/MediaQueryRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"


MediaQueryRuleset::MediaQueryRuleset(const LessSelector &selector,
                                     const LessRuleset &parent) :
                                     LessRuleset(selector, parent) {
}
MediaQueryRuleset::~MediaQueryRuleset() {
}

void MediaQueryRuleset::process(Stylesheet& s,
                                const Selector* prefix,
                                ProcessingContext& context) const {
  MediaQuery* query;
  Ruleset* target;
  Selector selector;
  Selector ruleSelector;

  selector = getSelector();
  context.processValue(selector);
  query = s.createMediaQuery(selector);
  
  if (prefix != NULL) {
    ruleSelector = *prefix;
    context.interpolate(ruleSelector);
    target = query->createRuleset(ruleSelector);

    processStatements(*target, &context);
  } else
    processStatements(*query, &context);
}
