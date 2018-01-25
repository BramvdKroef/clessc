#include "less/lessstylesheet/LessDeclaration.h"
#include "less/lessstylesheet/LessRuleset.h"

void LessDeclaration::setLessRuleset(LessRuleset &r) {
  lessRuleset = &r;
}
LessRuleset *LessDeclaration::getLessRuleset() {
  return lessRuleset;
}


void LessDeclaration::process(Ruleset &r, void* context) const {
  Declaration *d = r.createDeclaration();
  d->setProperty(property);
  d->setValue(value);

  ((ProcessingContext*)context)->interpolate(d->getProperty());
  ((ProcessingContext*)context)->processValue(d->getValue());
}
