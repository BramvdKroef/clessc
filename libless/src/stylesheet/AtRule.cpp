#include <less/stylesheet/Stylesheet.h>
#include <less/LogStream.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

AtRule::AtRule (const Token &keyword) {
  this->keyword = keyword;
}

AtRule::~AtRule() {
}

void AtRule::setKeyword (const Token &keyword) {
  this->keyword = keyword;
}
void AtRule::setRule(const TokenList &rule) {
  this->rule = rule;
}
Token& AtRule::getKeyword() {
  return keyword;
}
TokenList& AtRule::getRule() {
  return rule;
}

void AtRule::process(Stylesheet &s) {
  AtRule* target = s.createAtRule(keyword);
  target->setRule(rule);

  LogStream().notice(2) << "Processing @rule " << this->getKeyword() << ": " << this->getRule().toString();
}

void AtRule::write(CssWriter &writer) {
  writer.writeAtRule(keyword, rule);
}
