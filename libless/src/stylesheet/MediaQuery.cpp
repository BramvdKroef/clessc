#include <less/stylesheet/MediaQuery.h>

Selector& MediaQuery::getSelector() {
  return selector;
}
void MediaQuery::setSelector(const Selector &s) {
  selector = s;
}

MediaQuery* MediaQuery::createMediaQuery() {
  MediaQuery* q = getStylesheet()->createMediaQuery();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating media query";
#endif

  q->setSelector(getSelector());
  
  return q;
}

void MediaQuery::process(Stylesheet &s) {
  MediaQuery* query = s.createMediaQuery();
    
#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing media query " << getSelector().toString();
#endif
      
  query->setSelector(getSelector());
    
  Stylesheet::process(*query);
}

void MediaQuery::write(CssWriter &writer) {
  writer.writeMediaQueryStart(selector);
  Stylesheet::write(writer);
  
  writer.writeMediaQueryEnd();
}
