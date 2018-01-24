#include "less/lessstylesheet/LessDeclaration.h"
#include "less/lessstylesheet/LessRuleset.h"

void LessDeclaration::setLessRuleset(LessRuleset &r) {
  lessRuleset = &r;
}
LessRuleset *LessDeclaration::getLessRuleset() {
  return lessRuleset;
}


void LessDeclaration::process(Ruleset &r) {
  Declaration *d = r.createDeclaration();
  d->setProperty(property);
  d->setValue(value);

  getLessRuleset()->getContext()->interpolate(d->getProperty());
  getLessRuleset()->getContext()->processValue(d->getValue());
  
}
