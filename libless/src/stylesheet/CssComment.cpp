#include "less/stylesheet/CssComment.h"

CssComment::CssComment() {
  comment = "";
}

CssComment::CssComment(const Token &comment) {
  this->comment = comment;
}
void CssComment::setComment(const Token &comment) {
  this->comment = comment;
}
const Token &CssComment::getComment() const {
  return comment;
}

void CssComment::process(Ruleset &r, void* context) const {
  (void)context;
  CssComment *c = r.createComment();
  c->setComment(comment);
}

void CssComment::process(Stylesheet &s, void* context) const {
  (void)context;
  CssComment *c = s.createComment();
  c->setComment(comment);
}

void CssComment::write(CssWriter &writer) const {
  writer.writeComment(comment);
}
