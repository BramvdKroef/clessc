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
    
    //} else if (s.match(target)) {
    
    // add comma and selector
    //s.appendSelector(extension);
  }
}
void Extension::replaceInSelector(Selector &s) const {
  std::list<TokenList>::const_iterator it;

  for (it = target.getSelectors().begin();
       it != target.getSelectors().end(); it++) {
    s.replace(*it, extension);
  }
}
