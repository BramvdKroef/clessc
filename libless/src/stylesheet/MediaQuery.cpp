#include "less/stylesheet/MediaQuery.h"

Selector& MediaQuery::getSelector() {
  return selector;
}

const Selector& MediaQuery::getSelector() const {
  return selector;
}

void MediaQuery::setSelector(const Selector& s) {
  selector = s;
}

MediaQuery* MediaQuery::createMediaQuery() {
  MediaQuery* q = getStylesheet()->createMediaQuery();

  q->setSelector(getSelector());

  return q;
}

void MediaQuery::process(Stylesheet& s, void* context) const {
  MediaQuery* query = s.createMediaQuery();

  query->setSelector(getSelector());

  Stylesheet::process(*query, context);
}

void MediaQuery::write(CssWriter& writer) const {
  writer.writeMediaQueryStart(selector);
  Stylesheet::write(writer);

  writer.writeMediaQueryEnd();
}
