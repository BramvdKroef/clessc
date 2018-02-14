#include "less/stylesheet/MediaQuery.h"

const Token MediaQuery::BUILTIN_AND(
                                    "and", Token::IDENTIFIER, 0, 0, Token::BUILTIN_SOURCE);

MediaQuery::MediaQuery(const TokenList &selector) : selector(selector) {
}


TokenList& MediaQuery::getSelector() {
  return selector;
}

const TokenList& MediaQuery::getSelector() const {
  return selector;
}

MediaQuery* MediaQuery::createMediaQuery(const TokenList &selector) {
  TokenList s = selector;
  s.pop_front();

  s.push_front(BUILTIN_AND);
  s.push_front(Token::BUILTIN_SPACE);
  s.insert(s.begin(), getSelector().begin(), getSelector().end());
  
  return getStylesheet()->createMediaQuery(s);
}

void MediaQuery::process(Stylesheet& s, void* context) const {
  MediaQuery* query = s.createMediaQuery(getSelector());

  Stylesheet::process(*query, context);
}

void MediaQuery::write(CssWriter& writer) const {
  if (getStatements().empty())
    return;

  writer.writeMediaQueryStart(selector);
  Stylesheet::write(writer);

  writer.writeMediaQueryEnd();
}
