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

  // If the `important` flag is set, append '!important'
  if(((ProcessingContext*)context)->isImportant()) {
    
    if (d->getValue().size() < 3 ||
        d->getValue().back() == "important") {
      d->getValue().push_back(Token::BUILTIN_SPACE);
      d->getValue().push_back(Token::BUILTIN_IMPORTANT);
    }
    
  }
}
