/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

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
  if (in == NULL) 
    return;
  
  // Last char was a newline. Increment the line counter.
  if (lastReadEq('\n')) {
    line++;
    column = 0;
  }
    
  in->get(lastRead);

  // check for end of file or escape key
  if(in->eof() || lastRead == 27) 
    in = NULL;
  else if (in->fail() || in->bad())
    throw new IOException("Error reading input");

  if (!lastReadEq('\n')) // count chars that aren't newlines
    column++; 
}

Token::Type CssTokenizer::readNextToken(){
  if (in == NULL) {
    currentToken.type = Token::EOS;
    return Token::EOS;
  }

  currentToken.clear();
  switch (lastRead) {
  case '@':
    currentToken.type = Token::ATKEYWORD;
    currentToken.add(lastRead);
    readChar();
    if (!readIdent()) {
      currentToken.type = Token::OTHER;
    }
    break;
    
  case '#':
    currentToken.type = Token::HASH;
    currentToken.add(lastRead);
    readChar();
    if (!readName()) {
      throw new ParseException(&lastRead,
                               "name following '#'");
    }
    break;
    
  case '-':
    currentToken.add(lastRead);
    readChar();
    if (readNum(true)) {
      currentToken.type = Token::NUMBER;
      readNumSuffix();
    } else if (readIdent()) {
      currentToken.type = Token::IDENTIFIER;
    } else
      currentToken.type = Token::OTHER;
    break;
    
  case '~':
    currentToken.add(lastRead);
    readChar();
    if (lastRead == '=') {
      currentToken.add(lastRead);
      readChar();
      currentToken.type = Token::INCLUDES;
    } else
      currentToken.type = Token::OTHER;
    break;
    
  case '|':
    currentToken.add(lastRead);
    readChar();
    if (lastRead == '=') {
      currentToken.add(lastRead);
      readChar();
      currentToken.type = Token::DASHMATCH;
    } else
      currentToken.type = Token::OTHER;
    break;
    
  case '/':
    currentToken.add(lastRead);
    readChar();
    if (readComment()) 
      currentToken.type = Token::COMMENT;
    else
      currentToken.type = Token::OTHER;
    break;
    
  case ';':
    currentToken.type = Token::DELIMITER;
    currentToken.add(lastRead);
    readChar();
    break;
  case ':':
    currentToken.type = Token::COLON;
    currentToken.add(lastRead);
    readChar();
    break;
  case '{':
    currentToken.type = Token::BRACKET_OPEN;
    currentToken.add(lastRead);
    readChar();
    break;
  case '}':
    currentToken.type = Token::BRACKET_CLOSED;
    currentToken.add(lastRead);
    readChar();
    break;
  case '(':
    currentToken.type = Token::PAREN_OPEN;
    currentToken.add(lastRead);
    readChar();
    break;
  case ')':
    currentToken.type = Token::PAREN_CLOSED;
    currentToken.add(lastRead);
    readChar();
    break;
  case '[':
    currentToken.type = Token::BRACE_OPEN;
    currentToken.add(lastRead);
    readChar();
    break;
  case ']':
    currentToken.type = Token::BRACE_CLOSED;
    currentToken.add(lastRead);
    readChar();
    break;
    
  case '.':
    currentToken.add(lastRead);
    readChar();
    if (readNum(false)) {
      currentToken.type = Token::NUMBER;
      readNumSuffix();
    } 
    break;

  default:
    if (readString()) 
      currentToken.type = Token::STRING;
    else if (readNum(true)) {
      currentToken.type = Token::NUMBER;
      readNumSuffix();

    } else if (readIdent()) {
      currentToken.type = Token::IDENTIFIER;

      if (currentToken.str == "url" && readUrl())
        currentToken.type = Token::URL;
      else if (currentToken.str == "u" && lastReadEq('+')) {
        currentToken.add(lastRead);
        readChar();
        currentToken.type = Token::UNICODE_RANGE;
        readUnicodeRange();
      }
    } else if (readWhitespace()) {
      currentToken.type = Token::WHITESPACE;
      while (readWhitespace()) {};
    } else {
      currentToken.add(lastRead);
      readChar();
    }
    break;
  }

  return currentToken.type;
}


bool CssTokenizer::readIdent () {
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
  
  if (lastReadEq('_') ||
      lastReadInRange('a', 'z') ||
      lastReadInRange('A', 'Z')) {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return (readNonAscii() || readEscape());
}
bool CssTokenizer::readNonAscii () {
  if (in == NULL || lastRead >= 0)
    return false;
  
  currentToken.add(lastRead);
  readChar();
  return true;
}

bool CssTokenizer::readEscape () {
  if (!lastReadEq('\\'))
    return false;
  currentToken.add(lastRead);
  readChar();
  
  if (readUnicode()) 
    return true;    
  else if (!lastReadEq('\n') &&
           !lastReadEq('\r') &&
           !lastReadEq('\f')) {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return false;
}

bool CssTokenizer::readUnicode () {
  if (!lastReadIsHex())
    return false;

  // [0-9a-f]{1,6}(\r\n|[ \n\r\t\f])?
  for (int i=0; i < 6; i++) {
    currentToken.add(lastRead);
    readChar();
    if (readWhitespace() || !lastReadIsHex())
      break;
  }
  return true;
}

bool CssTokenizer::readNMChar () {
  if (in == NULL)
    return false;
  
  if (lastReadEq('_') ||
      lastReadInRange('a', 'z') ||
      lastReadInRange('A', 'Z') ||
      lastReadIsDigit() ||
      lastReadEq('-')) {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return (readNonAscii() || readEscape());
}

bool CssTokenizer::readNum (bool readDecimals) {
  if (!lastReadIsDigit())
    return false;
  while (lastReadIsDigit()) {
    currentToken.add(lastRead);
    readChar();
  }
  
  if (readDecimals && lastReadEq('.')) {
    currentToken.add(lastRead);
    readChar();

    while (lastReadIsDigit()) {
      currentToken.add(lastRead);
      readChar();
    }
  }
  return true;
}

bool CssTokenizer::readNumSuffix() {
  if (lastRead == '%') {
    currentToken.type = Token::PERCENTAGE;
    currentToken.add(lastRead);
    readChar();
    return true;
  } else if (readIdent())  {
    currentToken.type = Token::DIMENSION;
    return true;
  }
  // TODO: Support identifiers starting with '-'
  return false;
}

bool CssTokenizer::readString() {
  if (!lastReadEq('"') && !lastReadEq('\''))
    return false;
  char delim = lastRead;

  currentToken.add(lastRead);
  readChar();
  while (in != NULL) {
    if (lastReadEq(delim)) {
      currentToken.add(lastRead);
      readChar();
      return true;
    } else if (lastReadEq('\n') ||
               lastReadEq('\r') ||
               lastReadEq('\f')) {
      throw new ParseException("end of line",
                               "end of string");
    } else if (lastReadEq('\\'))
      // note that even though readEscape() returns false it still
      // eats the '\'.
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
  if (lastReadEq('\r')) {
    currentToken.add(lastRead);
    readChar();
    if (lastReadEq('\n')) {
      currentToken.add(lastRead);
      readChar();
    }
    return true;
  } else if (lastReadEq('\n') ||
             lastReadEq('\f')) {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return false;
}

bool CssTokenizer::readWhitespace () {
  if (lastReadEq(' ') ||
      lastReadEq('\t') ||
      lastReadEq('\r') ||
      lastReadEq('\n') ||
      lastReadEq('\f')) {
    currentToken.add(lastRead);
    readChar();
    return true;
  } else
    return false;
}

bool CssTokenizer::readUrl() {
  string urlchars = "!#$%&*-[]-~";
  
  if (!lastReadEq('('))
    return false;
  currentToken.add(lastRead);
  readChar();
  while(readWhitespace()) {};
    
  if (readString()) {
    if (lastReadEq(')')) {
      currentToken.add(lastRead);
      readChar();
      return true;
    } else {
      throw new ParseException(&lastRead,
                               "end of url (')')");
    }
  }

  while (in != NULL) {
    if (readWhitespace() || lastReadEq(')')) {
      while (readWhitespace()) {};
      if (lastReadEq(')')) {
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
  return false;
}


bool CssTokenizer::readComment () {
  if (!lastReadEq('*'))
    return false;
  currentToken.add(lastRead);
  readChar();
  while (in != NULL) {
    if (lastReadEq('*')) {
      currentToken.add(lastRead);
      readChar();
      
      if (lastReadEq('/')) {
        currentToken.add(lastRead);
        readChar();
        return true;
      }
      continue;
    }
    currentToken.add(lastRead);
    readChar();
  }
  throw new ParseException(&lastRead,
                           "end of comment (*/)");
  return false;
}

bool CssTokenizer::readUnicodeRange () {
  if (in == NULL)
    return false;
  for (int i=0; i < 6; i++) {
    if (!lastReadIsHex())
      break;
    currentToken.add(lastRead);
    readChar();
  }
  if (!lastReadEq('-'))
    return true;
  
  for (int i=0; i < 6; i++) {
    if (!lastReadIsHex())
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

bool CssTokenizer::lastReadEq(char c) {
  return (in != NULL && lastRead == c);
}

bool CssTokenizer::lastReadInRange(char c1, char c2) {
  return (in != NULL && lastRead >= c1 && lastRead <= c2);
}
bool CssTokenizer::lastReadIsDigit() {
  return (in != NULL &&
          lastReadInRange('0', '9'));
}
bool CssTokenizer::lastReadIsHex() {
  return (in != NULL &&
          (lastReadIsDigit() ||
           lastReadInRange('a', 'f') ||
           lastReadInRange('A', 'F')));
}
