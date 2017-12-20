#include <less/stylesheet/MediaQuery.h>
#include <less/LogStream.h>

Selector& MediaQuery::getSelector() {
  return selector;
}
void MediaQuery::setSelector(const Selector &s) {
  selector = s;
}

MediaQuery* MediaQuery::createMediaQuery() {
  MediaQuery* q = getStylesheet()->createMediaQuery();

  LogStream().notice(3) << "Creating media query";

  q->setSelector(getSelector());
  
  return q;
}

void MediaQuery::process(Stylesheet &s) {
  MediaQuery* query = s.createMediaQuery();

  LogStream().notice(2) << "Processing media query " << getSelector().toString();

  query->setSelector(getSelector());
    
  Stylesheet::process(*query);
}

void MediaQuery::write(CssWriter &writer) {
  writer.writeMediaQueryStart(selector);
  Stylesheet::write(writer);
  
  writer.writeMediaQueryEnd();
}
