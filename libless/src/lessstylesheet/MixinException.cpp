#include "less/lessstylesheet/MixinException.h"

MixinException::MixinException(const Mixin &mixin):
  LessException(mixin.name.front()), mixin(&mixin) {
  err.append("No ruleset definition found for: ");
  err.append(mixin.name.toString());
}
const char* MixinException::what() const throw() {
  return err.c_str();
}
