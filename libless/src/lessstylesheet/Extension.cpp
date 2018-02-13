#include "less/lessstylesheet/Extension.h"

Extension::Extension(): all(false){
}

Extension::~Extension() {
}

Selector &Extension::getTarget() {
  return target;
}

Selector &Extension::getExtension() {
  return extension;
}

const Selector &Extension::getTarget() const {
  return target;
}

const Selector &Extension::getExtension() const {
  return extension;
}

void Extension::setExtension(const Selector &s) {
  extension = s;
}


void Extension::setAll(bool b) {
  all = b;
}
bool Extension::isAll() const {
  return all;
}


void Extension::updateSelector(Selector &s) const {
  if (isAll()) {
    replaceInSelector(s);
    
  } else if (s.match(target)) {
    s.appendSelector(extension);
  }
}
void Extension::replaceInSelector(Selector &s) const {
  std::list<TokenList>::const_iterator it1, it2;

  for (it1 = target.begin();
       it1 != target.end(); it1++) {

    for (it2 = extension.begin();
         it2 != extension.end(); it2++) {

      // If no matches are found, there is no need to try other extension
      // selectors.
      if (! s.replace(*it1, *it2))
        break;
    }
  }
}
