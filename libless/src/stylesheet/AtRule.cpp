#include "less/stylesheet/AtRule.h"

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
const Token &AtRule::getKeyword() const {
  return keyword;
}
TokenList &AtRule::getRule() {
  return rule;
}

const TokenList &AtRule::getRule() const {
  return rule;
}

void AtRule::process(Stylesheet &s, void* context) const {
  (void)context;
  
  AtRule *target = s.createAtRule(keyword);
  target->setRule(rule);
}

void AtRule::write(CssWriter &writer) const {
  writer.writeAtRule(keyword, rule);
}
