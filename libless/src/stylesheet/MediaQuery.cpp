#include "less/stylesheet/MediaQuery.h"


Selector& MediaQuery::getSelector() {
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

void MediaQuery::process(Stylesheet& s) {
  MediaQuery* query = s.createMediaQuery();

  query->setSelector(getSelector());

  Stylesheet::process(*query);
}

void MediaQuery::write(CssWriter& writer) {
  writer.writeMediaQueryStart(selector);
  Stylesheet::write(writer);

  writer.writeMediaQueryEnd();
}
