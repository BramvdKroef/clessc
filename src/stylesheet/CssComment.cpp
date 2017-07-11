
#include "CssComment.h"

CssComment::CssComment() {
  comment = "";
}

CssComment::CssComment(const Token &comment) {
  this->comment = comment;
}
void CssComment::setComment(const Token &comment) {
  this->comment = comment;
}
Token& CssComment::getComment() {
  return comment;
}

void CssComment::process(Ruleset &r) {
  CssComment* c = r.createComment();
  c->setComment(comment);
}

void CssComment::process(Stylesheet &s) {
  CssComment* c = s.createComment();
  c->setComment(comment);
}

void CssComment::write(CssWriter &writer) {
  writer.writeComment(comment);
}


