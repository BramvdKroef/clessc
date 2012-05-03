#include "CssTokenizer.h"

CssTokenizer::CssTokenizer(istream* in){
  this->in = in;
  line = 1;
  column = 0;
  readChar();
}

CssTokenizer::~CssTokenizer(){
}

void CssTokenizer::readChar(){
  if (in != NULL) {
    // Last char was a newline. Increment the line counter.
    if (lastRead == '\n') {
      line++;
      column = 0;
    }
    
    in->get(lastRead);

    // check for end of file or escape key
    if(in->eof() || lastRead == 27) 
      in = NULL;
    else if (in->fail() || in->bad())
      throw new IOException("Error reading input");

    if (lastRead != '\n') // count chars that aren't newlines
      column++; 
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
  } else if (lastRead == '-') {
    currentToken.add(lastRead);
    readChar();
    if (readNum()) {
      currentToken.type = Token::NUMBER;
      if (lastRead == '%') {
        currentToken.type = Token::PERCENTAGE;
        currentToken.add(lastRead);
        readChar();
      } else if (readIdent()) 
        currentToken.type = Token::DIMENSION;
    } else if (readIdent()) {
      currentToken.type = Token::IDENTIFIER;
      if (lastRead == '(') {
        currentToken.add(lastRead);
        readChar();
        currentToken.type = Token::FUNCTION;
      }
    } else
      currentToken.type = Token::OTHER;
  } else if (readNum()) {
    currentToken.type = Token::NUMBER;
    if (lastRead == '%') {
      currentToken.type = Token::PERCENTAGE;
      currentToken.add(lastRead);
      readChar();
    } else if (readIdent()) 
      currentToken.type = Token::DIMENSION;
    
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
  if (in == NULL)
    return false;
  
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
  if (in == NULL)
    return false;
  
  if (lastRead == '_' ||
      (lastRead >= 'a' && lastRead <= 'z') ||
      (lastRead >= 'A' && lastRead <= 'Z')) {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return (readNonAscii() || readEscape());
}
bool CssTokenizer::readNonAscii () {
  if (in == NULL)
    return false;
  
  if (lastRead >= 0) {
    return false;
  } else {
    currentToken.add(lastRead);
    readChar();
    return true;
  } 
}
bool CssTokenizer::readEscape () {
  if (in == NULL || lastRead != '\\')
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
  if (in == NULL || !isHex(lastRead))
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
  if (in == NULL)
    return false;
  
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
  if (in == NULL || (!isdigit(lastRead) && lastRead != '.'))
    return false;
  while (in != NULL && isdigit(lastRead)) {
    currentToken.add(lastRead);
    readChar();
  }
  if (in != NULL && lastRead == '.') {
    currentToken.add(lastRead);
    readChar();

    while (in != NULL && isdigit(lastRead)) {
      currentToken.add(lastRead);
      readChar();
    }
  }
  return true;
}

bool CssTokenizer::readString() {
  if (in == NULL || (lastRead != '"' && lastRead != '\''))
    return false;
  char delim = lastRead;

  currentToken.add(lastRead);
  readChar();
  while (in != NULL) {
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
  throw new ParseException("end of input",
                           "end of string");
  return false;
}

bool CssTokenizer::readNewline () {
  if (in == NULL)
    return false;
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
  if (in == NULL)
    return false;
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
  
  if (in == NULL || lastRead != '(') 
    return false;
  currentToken.add(lastRead);
  readChar();
  while(readWhitespace()) {};
    
  if (readString()) {
    if (in != NULL && lastRead == ')') {
      currentToken.add(lastRead);
      readChar();
      return true;
    } else {
      throw new ParseException(&lastRead,
                               "end of url (')')");
    }
  }

  while (in != NULL) {
    if (readWhitespace() || (in != NULL && lastRead == ')')) {
      while (readWhitespace()) {};
      if (in != NULL && lastRead == ')') {
        currentToken.add(lastRead);
        readChar();
        return true;
      } else {
        throw new ParseException(&lastRead,
                                 "end of url (')')");
      }
    } else if (in != NULL && urlchars.find(lastRead)) {
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
  if (in == NULL || lastRead != '*')
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
  if (in == NULL)
    return false;
  for (int i=0; i < 6; i++) {
    if (in == NULL || !isHex(lastRead))
      break;
    currentToken.add(lastRead);
    readChar();
  }
  if (in == NULL || lastRead != '-')
    return true;
  
  for (int i=0; i < 6; i++) {
    if (in == NULL || !isHex(lastRead))
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

unsigned int CssTokenizer::getLineNumber(){
  return line;
}
unsigned int CssTokenizer::getColumn(){
  return column;
}
