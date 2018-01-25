#include "less/lessstylesheet/MediaQueryRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"

const Token MediaQueryRuleset::BUILTIN_AND(
    "and", Token::IDENTIFIER, 0, 0, Token::BUILTIN_SOURCE);

MediaQueryRuleset::MediaQueryRuleset() : LessRuleset() {
}
MediaQueryRuleset::~MediaQueryRuleset() {
}

void MediaQueryRuleset::process(Stylesheet& s,
                                Selector* prefix,
                                ProcessingContext& context) {
  MediaQuery* query;
  Ruleset* target;
  Selector selector;

  query = s.createMediaQuery();
  selector = getSelector();
  context.processValue(selector);

  if (query->getSelector().size() > 0) {
    selector.pop_front();

    query->getSelector().push_back(Token::BUILTIN_SPACE);
    query->getSelector().push_back(BUILTIN_AND);
    query->getSelector().insert(
        query->getSelector().end(), selector.begin(), selector.end());
  } else
    query->setSelector(selector);

  if (prefix != NULL) {
    target = query->createRuleset();
    target->setSelector(*prefix);

    context.interpolate(target->getSelector());

    processStatements(*target, &context);
  } else
    processStatements(*query, &context);
}
