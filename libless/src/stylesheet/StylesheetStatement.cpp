#include "less/stylesheet/StylesheetStatement.h"

void StylesheetStatement::setStylesheet(Stylesheet* s) {
  stylesheet = s;
}
Stylesheet* StylesheetStatement::getStylesheet() const {
  return stylesheet;
}

void StylesheetStatement::setReference(bool ref) {
  reference = ref;
}
bool StylesheetStatement::isReference() const {
  return reference;
}
