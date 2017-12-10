#include "less/stylesheet/Declaration.h"
#include "less/LogStream.h"

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
TokenList &Declaration::getValue() {
  return value;
}

void Declaration::process(Ruleset &r) {
  LogStream().notice(2) << "Processing declaration " << property << ": "
                        << value.toString();
  Declaration *d = r.createDeclaration();
  d->setProperty(property);
  d->setValue(value);
}

void Declaration::write(CssWriter &writer) {
  writer.writeDeclaration(property, value);
}
