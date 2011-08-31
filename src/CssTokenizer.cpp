#include "CssTokenizer.h"

CssTokenizer::CssTokenizer(istream* in){
  this->in = in;
  lineNum = 1;
  position = 0;
  readChar();
}

CssTokenizer::~CssTokenizer(){
}

void CssTokenizer::readChar(){
  if (in != NULL) {
    in->get(lastRead);

    // check for end of file or escape key
    if(in->eof() || lastRead == 27) {
      in = NULL;
      lastRead = -1;
    } else if (in->fail() || in->bad())
      throw new IOException("Error reading input");
    
    position++;
    if (lastRead == '\n') {
      lineNum++;
      position = 1;
    }
  }
}

Token::Type CssTokenizer::readNextToken(){
  if (in == NULL) {
    currentToken.type = Token::EOS;
    return Token::EOS;
  }

  currentToken.clear();
  
  if (lastRead == '@') {
    currentToken.type = Token::ATKEYWORD;
    currentToken.add(lastRead);
    readChar();
    if (!readIdent()) {
      currentToken.type = Token::OTHER;
    }
  } else if (lastRead == '#') {
    currentToken.type = Token::HASH;
    currentToken.add(lastRead);
    readChar();
    if (!readName()) {
      throw new ParseException(&lastRead,
                               "name following '#'");
    }
  } else if (readString()) {
    currentToken.type = Token::STRING;
  } else if (readNum()) {
    currentToken.type = Token::NUMBER;
    if (lastRead == '%') {
      currentToken.type = Token::PERCENTAGE;
      currentToken.add(lastRead);
      readChar();
    } else if (readIdent()) {
      currentToken.type = Token::DIMENSION;
    }
  } else if (readIdent()) {
    currentToken.type = Token::IDENTIFIER;

    if (currentToken.str == "url" && readUrl())
      currentToken.type = Token::URL;
    else if (currentToken.str == "u" && lastRead == '+') {
      currentToken.add(lastRead);
      readChar();
      currentToken.type = Token::UNICODE_RANGE;
      readUnicodeRange();
    } else if (lastRead == '(') {
      currentToken.add(lastRead);
      readChar();
      currentToken.type = Token::FUNCTION;
    }
  } else if (readWhitespace()) {
    currentToken.type = Token::WHITESPACE;
    while (readWhitespace()) {};
  } else if (lastRead == '~') {
    currentToken.add(lastRead);
    readChar();
    if (lastRead == '=') {
      currentToken.add(lastRead);
      readChar();
      currentToken.type = Token::INCLUDES;
    } else
      currentToken.type = Token::OTHER;
  } else if (lastRead == '|') {
    currentToken.add(lastRead);
    readChar();
    if (lastRead == '=') {
      currentToken.add(lastRead);
      readChar();
      currentToken.type = Token::DASHMATCH;
    } else
      currentToken.type = Token::OTHER;
  } else if (lastRead == '/') {
    currentToken.add(lastRead);
    readChar();
    if (readComment()) 
      currentToken.type = Token::COMMENT;
    else
      currentToken.type = Token::OTHER;
  } else {
    switch (lastRead) {
    case ';':
      currentToken.type = Token::DELIMITER;
      break;
    case ':':
      currentToken.type = Token::COLON;
      break;
    case '{':
      currentToken.type = Token::BRACKET_OPEN;
      break;
    case '}':
      currentToken.type = Token::BRACKET_CLOSED;
      break;
    case '(':
      currentToken.type = Token::PAREN_OPEN;
      break;
    case ')':
      currentToken.type = Token::PAREN_CLOSED;
      break;
    case '[':
      currentToken.type = Token::BRACE_OPEN;
      break;
    case ']':
      currentToken.type = Token::BRACE_CLOSED;
      break;
    }
    currentToken.add(lastRead);
    readChar();
  }
  return currentToken.type;
}


bool CssTokenizer::readIdent () {
  if (lastRead == '-') {
    currentToken.add(lastRead);
    readChar();
  }
  if (!readNMStart())
    return false;
  else
    while (readNMChar()) {}
  return true;
}

bool CssTokenizer::readName () {
  if (!readNMChar())
    return false;
  while (readNMChar()) {}
  return true;
}

bool CssTokenizer::readNMStart () {
  if (lastRead == '_' ||
      (lastRead >= 'a' && lastRead <= 'z') ||
      (lastRead >= 'F' && lastRead <= 'Z')) {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return (readNonAscii() || readEscape());
}
bool CssTokenizer::readNonAscii () {
  if (lastRead > 237) {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return false;
}
bool CssTokenizer::readEscape () {
  if (lastRead != '\\')
    return false;
  currentToken.add(lastRead);
  readChar();
  
  if (readUnicode()) 
    return true;    
  else if (lastRead != '\n' ||
           lastRead != '\r' ||
           lastRead != '\f') {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return false;
}
bool CssTokenizer::readUnicode () {
  if (!isHex(lastRead))
    return false;

  // [0-9a-f]{1,6}(\r\n|[ \n\r\t\f])?
  for (int i=0; i < 6; i++) {
    if (readWhitespace())
      break;
    currentToken.add(lastRead);
    readChar();
    if (!isHex(lastRead)) {
      /*throw new ParseException(&lastRead,
        "hex code(0-9a-f) in unicode character");*/
      /* Just ignore and assume the unicode stops here  */
      break;
    }
  }
  return true;
}

bool CssTokenizer::readNMChar () {
  if (lastRead == '_' ||
      (lastRead >= 'a' && lastRead <= 'z') ||
      (lastRead >= 'A' && lastRead <= 'Z') ||
      isdigit(lastRead) ||
      lastRead == '-') {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return (readNonAscii() || readEscape());
}

bool CssTokenizer::readNum () {
  if (!isdigit(lastRead) && lastRead != '.')
    return false;
  while (isdigit(lastRead)) {
    currentToken.add(lastRead);
    readChar();
  }
  if (lastRead == '.') {
    currentToken.add(lastRead);
    readChar();

    while (isdigit(lastRead)) {
      currentToken.add(lastRead);
      readChar();
    }
  }
  return true;
}

bool CssTokenizer::readString() {
  if (lastRead != '"' && lastRead != '\'')
    return false;
  char delim = lastRead;

  currentToken.add(lastRead);
  readChar();
  while (lastRead != -1) {
    if (lastRead == delim) {
      currentToken.add(lastRead);
      readChar();
      return true;
    } else if (lastRead == '\n' ||
        lastRead == '\r' ||
        lastRead == '\f') {
      throw new ParseException("end of line",
                               "end of string");
    } else if (lastRead == '\\') 
      readEscape() || readNewline();
    else {
      currentToken.add(lastRead);
      readChar();
    }
  }
  throw new ParseException(&lastRead,
                           "end of string");
  return true;
}

bool CssTokenizer::readNewline () {
  if (lastRead == '\r') {
    currentToken.add(lastRead);
    readChar();
    if (lastRead == '\n') {
      currentToken.add(lastRead);
      readChar();
    }
    return true;
  } else if (lastRead == '\n' ||
             lastRead == '\f') {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return false;
}

bool CssTokenizer::readWhitespace () {
  if (lastRead != ' ' &&
      lastRead != '\t' &&
      lastRead != '\r' &&
      lastRead != '\n' &&
      lastRead != '\f')
    return false;
  
  currentToken.add(lastRead);
  readChar();
  return true;
}

bool CssTokenizer::readUrl() {
  string urlchars = "!#$%&*-[]-~";
  
  if (lastRead != '(') 
    return false;
  currentToken.add(lastRead);
  readChar();
  while(readWhitespace()) {};
    
  if (readString()) {
    if (lastRead == ')') {
      currentToken.add(lastRead);
      readChar();
      return true;
    } else {
      throw new ParseException(&lastRead,
                               "end of url (')')");
    }
  }

  while (lastRead != -1) {
    if (readWhitespace() || lastRead == ')') {
      while (readWhitespace()) {};
      if (lastRead == ')') {
        currentToken.add(lastRead);
        readChar();
        return true;
      } else {
        throw new ParseException(&lastRead,
                                 "end of url (')')");
      }
    } else if (urlchars.find(lastRead)) {
      currentToken.add(lastRead);
      readChar();
    } else if (!readNonAscii() &&
               !readEscape()) {
      throw new ParseException(&lastRead,
                               "end of url (')')");
    }
  }
  throw new ParseException(&lastRead,
                           "end of url (')')");
  return true;
}


bool CssTokenizer::isHex(char c) {
  return isdigit(c) ||
    (c >= 'a' && c <= 'f') ||
    (c >= 'A' && c <= 'F');
}


bool CssTokenizer::readComment () {
  if (lastRead != '*')
    return false;
  currentToken.add(lastRead);
  readChar();
  while (in != NULL) {
    if (lastRead == '*') {
      currentToken.add(lastRead);
      readChar();
      
      if (lastRead == '/') {
        currentToken.add(lastRead);
        readChar();
        return true;
      }
    }
    currentToken.add(lastRead);
    readChar();
  }
  throw new ParseException(&lastRead,
                           "end of comment (*/)");
  return true;
}

bool CssTokenizer::readUnicodeRange () {
  for (int i=0; i < 6; i++) {
    if (isHex(lastRead))
      break;
    currentToken.add(lastRead);
    readChar();
  }
  if (lastRead != '-')
    return true;
  
  for (int i=0; i < 6; i++) {
    if (isHex(lastRead))
      break;
    currentToken.add(lastRead);
    readChar();
  }
  return true;
}

Token* CssTokenizer::getToken(){
  return &currentToken;
}
Token::Type CssTokenizer::getTokenType() {
  return currentToken.type;
}

int CssTokenizer::getLineNumber(){
  return lineNum;
}
int CssTokenizer::getPosition(){
  return position;
}
