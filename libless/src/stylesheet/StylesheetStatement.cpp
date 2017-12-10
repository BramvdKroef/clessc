#include "less/stylesheet/Stylesheet.h"

void StylesheetStatement::setStylesheet(Stylesheet* s) {
  stylesheet = s;
}
Stylesheet* StylesheetStatement::getStylesheet() {
  return stylesheet;
}

void StylesheetStatement::setReference(bool ref) {
  reference = ref;
}
bool StylesheetStatement::isReference() {
  return reference;
}
