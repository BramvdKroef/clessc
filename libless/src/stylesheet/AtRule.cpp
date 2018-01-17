#include "less/stylesheet/Stylesheet.h"

AtRule::AtRule(const Token &keyword) {
  this->keyword = keyword;
}

AtRule::~AtRule() {
}

void AtRule::setKeyword(const Token &keyword) {
  this->keyword = keyword;
}
void AtRule::setRule(const TokenList &rule) {
  this->rule = rule;
}
Token &AtRule::getKeyword() {
  return keyword;
}
TokenList &AtRule::getRule() {
  return rule;
}

void AtRule::process(Stylesheet &s) {
  AtRule *target = s.createAtRule(keyword);
  target->setRule(rule);
}

void AtRule::write(CssWriter &writer) {
  writer.writeAtRule(keyword, rule);
}
