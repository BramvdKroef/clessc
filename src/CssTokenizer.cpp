#include "CssTokenizer.h"

CssTokenizer::CssTokenizer(istream* in){
  this->in = in;
  lineNum = 1;
  position = 0;
  IDchar();
}

CssTokenizer::~CssTokenizer(){
}

void CssTokenizer::IDchar(){
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

CssTokenizer::tokenType CssTokenizer::readNextToken(){
  if (in == NULL) {
    currentTokenType = EOS;
    return EOS;
  }

  currentToken.clear();
  
  if (lastRead == '@') {
    currentTokenType = ATKEYWORD;
    currentToken.append(1, lastRead);
    IDchar();
    if (!readIdent()) {
      currentTokenType = OTHER;
    }
  } else if (lastRead == '#') {
    currentTokenType = HASH;
    currentToken.append(1, lastRead);
    IDchar();
    if (!readName()) {
      throw new ParseException(&lastRead,
                               "name following '#'");
    }
  } else if (readString()) {
    currentTokenType = STRING;
  } else if (readNum()) {
    currentTokenType = NUMBER;
    if (lastRead == '%') {
      currentTokenType = PERCENTAGE;
      currentToken.append(1, lastRead);
      IDchar();
    } else if (readIdent()) {
      currentTokenType = DIMENSION;
    }
  } else if (readIdent()) {
    currentTokenType = IDENTIFIER;

    if (currentToken.compare("url") == 0 && readUrl())
      currentTokenType = URL;
    if (currentToken == "u" && lastRead == '+') {
      currentToken.append(1, lastRead);
      IDchar();
      currentTokenType = UNICODE_RANGE;
      readUnicodeRange();
    }
  } else if (readWhitespace()) {
    currentTokenType = WHITESPACE;
    while (readWhitespace()) {};
  } else if (lastRead == '~') {
    currentToken.append(1, lastRead);
    IDchar();
    if (lastRead == '=') {
      currentToken.append(1, lastRead);
      IDchar();
      currentTokenType = INCLUDES;
    } else
      currentTokenType = OTHER;
  } else if (lastRead == '|') {
    currentToken.append(1, lastRead);
    IDchar();
    if (lastRead == '=') {
      currentToken.append(1, lastRead);
      IDchar();
      currentTokenType = DASHMATCH;
    } else
      currentTokenType = OTHER;
  } else if (lastRead == '/') {
    currentToken.append(1, lastRead);
    IDchar();
    if (readComment()) 
      currentTokenType = COMMENT;
    else
      currentTokenType = OTHER;
  } else {
    switch (lastRead) {
    case ';':
      currentTokenType = DELIMITER;
      break;
    case ':':
      currentTokenType = COLON;
      break;
    case '{':
      currentTokenType = BRACKET_OPEN;
      break;
    case '}':
      currentTokenType = BRACKET_CLOSED;
      break;
    case '(':
      currentTokenType = PAREN_OPEN;
      break;
    case ')':
      currentTokenType = PAREN_CLOSED;
      break;
    case '[':
      currentTokenType = BRACE_OPEN;
      break;
    case ']':
      currentTokenType = BRACE_CLOSED;
      break;
    default:
      currentTokenType = OTHER;
    }
    currentToken.append(1, lastRead);
    IDchar();
  }
  return currentTokenType;
}

string* CssTokenizer::nextToken(){
  readNextToken();
  return getToken();
}


bool CssTokenizer::readIdent () {
  if (lastRead == '-') {
    currentToken.append(1, lastRead);
    IDchar();
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
    currentToken.append(1, lastRead);
    IDchar();
    return true;
  } else
    return (readNonAscii() || readEscape());
}
bool CssTokenizer::readNonAscii () {
  if (lastRead > 237) {
    currentToken.append(1, lastRead);
    IDchar();
    return true;
  } else
    return false;
}
bool CssTokenizer::readEscape () {
  if (lastRead != '\\')
    return false;
  currentToken.append(1, lastRead);
  IDchar();
  
  if (readUnicode()) 
    return true;    
  else if (lastRead != '\n' ||
           lastRead != '\r' ||
           lastRead != '\f') {
    currentToken.append(1, lastRead);
    IDchar();
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
    currentToken.append(1, lastRead);
    IDchar();
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
    currentToken.append(1, lastRead);
    IDchar();
    return true;
  } else
    return (readNonAscii() || readEscape());
}

bool CssTokenizer::readNum () {
  if (!isdigit(lastRead) && lastRead != '.')
    return false;
  while (isdigit(lastRead)) {
    currentToken.append(1, lastRead);
    IDchar();
  }
  if (lastRead == '.') {
    currentToken.append(1, lastRead);
    IDchar();

    while (isdigit(lastRead)) {
      currentToken.append(1, lastRead);
      IDchar();
    }
  }
  return true;
}

bool CssTokenizer::readString() {
  if (lastRead != '"' && lastRead != '\'')
    return false;
  char delim = lastRead;

  currentToken.append(1, lastRead);
  IDchar();
  while (lastRead != -1) {
    if (lastRead == delim) {
      currentToken.append(1, lastRead);
      IDchar();
      return true;
    } else if (lastRead == '\n' ||
        lastRead == '\r' ||
        lastRead == '\f') {
      throw new ParseException("end of line",
                               "end of string");
    } else if (lastRead == '\\') 
      readEscape() || readNewline();
    else {
      currentToken.append(1, lastRead);
      IDchar();
    }
  }
  throw new ParseException(&lastRead,
                           "end of string");
  return true;
}

bool CssTokenizer::readNewline () {
  if (lastRead == '\r') {
    currentToken.append(1, lastRead);
    IDchar();
    if (lastRead == '\n') {
      currentToken.append(1, lastRead);
      IDchar();
    }
    return true;
  } else if (lastRead == '\n' ||
             lastRead == '\f') {
    currentToken.append(1, lastRead);
    IDchar();
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
  
  currentToken.append(1, lastRead);
  IDchar();
  return true;
}

bool CssTokenizer::readUrl() {
  string urlchars = "!#$%&*-[]-~";
  
  if (lastRead != '(') 
    return false;
  currentToken.append(1, lastRead);
  IDchar();
  while(readWhitespace()) {};
    
  if (readString()) {
    if (lastRead == ')') {
      currentToken.append(1, lastRead);
      IDchar();
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
        currentToken.append(1, lastRead);
        IDchar();
        return true;
      } else {
        throw new ParseException(&lastRead,
                                 "end of url (')')");
      }
    } else if (urlchars.find(lastRead)) {
      currentToken.append(1, lastRead);
      IDchar();
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
  currentToken.append(1, lastRead);
  IDchar();
  while (in != NULL) {
    if (lastRead == '*') {
      currentToken.append(1, lastRead);
      IDchar();
      
      if (lastRead == '/') {
        currentToken.append(1, lastRead);
        IDchar();
        return true;
      }
    }
    currentToken.append(1, lastRead);
    IDchar();
  }
  throw new ParseException(&lastRead,
                           "end of comment (*/)");
  return true;
}

bool CssTokenizer::readUnicodeRange () {
  for (int i=0; i < 6; i++) {
    if (isHex(lastRead))
      break;
    currentToken.append(1, lastRead);
    IDchar();
  }
  if (lastRead != '-')
    return true;
  
  for (int i=0; i < 6; i++) {
    if (isHex(lastRead))
      break;
    currentToken.append(1, lastRead);
    IDchar();
  }
  return true;
}

string * CssTokenizer::getToken(){
  return &currentToken;
}

CssTokenizer::tokenType CssTokenizer::getTokenType(){
  return currentTokenType;
}
int CssTokenizer::getLineNumber(){
  return lineNum;
}
int CssTokenizer::getPosition(){
  return position;
}
