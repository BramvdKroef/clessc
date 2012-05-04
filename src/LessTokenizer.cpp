#include "LessTokenizer.h"


bool LessTokenizer::readComment () {
  if (!lastReadEq('/'))
    return CssTokenizer::readComment();

  currentToken.add(lastRead);
  readChar();
  while (in != NULL && !lastReadEq('\n')) {
    currentToken.add(lastRead);
    readChar();
  }
  return true;
}
