#include "LessTokenizer.h"


bool LessTokenizer::readComment () {
  if (lastRead != '/')
    return CssTokenizer::readComment();

  currentToken.add(lastRead);
  readChar();
  while (in != NULL) {
    currentToken.add(lastRead);
    readChar();
    if (lastRead == '\n') 
      break;
  }
  return true;
}
