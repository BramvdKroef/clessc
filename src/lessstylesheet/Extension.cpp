
#include "Extension.h"

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

Extension::Extension() {
  all = false;
}

Extension::Extension(bool all) {
  all = true;
}
Extension::~Extension() {
}

Selector& Extension::getTarget() {
  return target;
}
Selector& Extension::getExtension() {
  return extension;
}

void Extension::setExtension(Selector &selector) {
  extension = selector;
}

void Extension::updateSelector(Selector &s) const {
  if (target.back() == "all") {
    replaceInSelector(s);
  } else if (s.match(target)) {

#ifdef WITH_LIBGLOG
    VLOG(2) << "Extending " << s.toString() << " with " << extension.toString() ;
#endif

    // add comma and selector    
    s.push_back(Token::BUILTIN_COMMA);
    s.insert(s.end(), extension.begin(), extension.end());
  }
}
void Extension::replaceInSelector(Selector &s) const {
  Selector t = target;
  Selector::const_iterator end, pos, first, last;
  t.pop_back();
  t.rtrim();

  end = s.end();

  for (first = s.begin(); first != end;) {
    last = s.findComma(first, end);
    pos = s.find(t, first, last);

    if (pos != last) {
#ifdef WITH_LIBGLOG
      VLOG(2) << "Extending " << s.toString() << " with " << extension.toString() ;
#endif
      
      s.push_back(Token::BUILTIN_COMMA);

      // fix position of <code>last</code>. If <code>last ==
      // end</code> it moves up as tokens are inserted, and we want it
      // to stay where it is.
      if (last == end)
        last--;
      
      s.insert(s.end(), first, pos);
      s.insert(s.end(), extension.begin(), extension.end());
      pos = s.walk(t, pos);
      s.insert(s.end(), pos, last);
    }
    
    first = last;
    if (first != end)
      first++;
  }
}
