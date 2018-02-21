#include "less/stylesheet/Declaration.h"
 
Declaration::Declaration() {
  property = "";
}

Declaration::Declaration(const Token &property) {
  this->property = property;
}

Declaration::~Declaration() {
}

void Declaration::setProperty(const Token &property) {
  this->property = property;
}
void Declaration::setValue(const TokenList &value) {
  this->value = value;
}
Token &Declaration::getProperty() {
  return property;
}

const Token &Declaration::getProperty() const {
  return property;
}

TokenList &Declaration::getValue() {
  return value;
}

const TokenList &Declaration::getValue() const {
  return value;
}

void Declaration::process(Ruleset &r, void* context) const {
  (void)context;
  Declaration *d = r.createDeclaration();
  d->setProperty(property);
  d->setValue(value);
}

void Declaration::write(CssWriter &writer) const {
  writer.writeDeclaration(property, value);
}
