#include "less/lessstylesheet/LessAtRule.h"
#include "less/lessstylesheet/LessStylesheet.h"

LessAtRule::LessAtRule(const Token &keyword): AtRule(keyword) {
}
LessAtRule::~LessAtRule() {
}

void LessAtRule::setLessStylesheet(LessStylesheet &s) {
  lessStylesheet = &s;
}

LessStylesheet* LessAtRule::getLessStylesheet() {
  return lessStylesheet;
}


void LessAtRule::process(Stylesheet &s) {
  AtRule* target = s.createAtRule(getKeyword());
  
  target->setRule(getRule());

  getLessStylesheet()->getContext()->processValue(target->getRule());
}
