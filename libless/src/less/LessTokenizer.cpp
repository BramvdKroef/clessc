#include "less/less/LessTokenizer.h"

LessTokenizer::~LessTokenizer() {
}

bool LessTokenizer::readComment() {
  if (!lastReadEq('/'))
    return CssTokenizer::readComment();

  currentToken.append(lastRead);
  readChar();
  while (in != NULL && !lastReadEq('\n')) {
    currentToken.append(lastRead);
    readChar();
  }
  return true;
}
