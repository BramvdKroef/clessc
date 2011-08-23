#ifndef __Tokenizer_h__
#define __Tokenizer_h__

#include <ctype.h>
#include <string>
#include <iostream>
#include "IOException.h"
#include "ParseException.h"

using namespace std;


/**
 * Leest char's uit een implementatie van Reader en herkent de typen zoals
 * hieronder aangegeven. Er worden char's gelezen totdat het laastgelezen char
 * een ander type heeft dan de vorigen. Spaties worden niet gezien als token. In
 * een getal mag ook een punt voorkomen.
 *
 * Met getTokenType() kan het type van het token gelezen worden en met
 * getToken() kan een pointer naar het token verkregen worden.
 *
 * Elke keer als de Tokenizer een End Of Line(TT_EOL) token tegenkomt word
 * lineno met 1 verhoogd.
 */
class Tokenizer {
public:
  enum tokenType{ATKEYWORD, HASH, STRING, NUMBER, PERCENTAGE, DIMENSION,
                 IDENTIFIER, URL, WHITESPACE, EOS, UNICODE_RANGE, OTHER,
                 DASHMATCH, DELIMITER, COLON, BRACKET_OPEN, BRACKET_CLOSED,
                 PAREN_OPEN, PAREN_CLOSED, BRACE_OPEN, BRACE_CLOSED, INCLUDES,
                 COMMENT}; 
	
  Tokenizer(istream* in);
		
  ~Tokenizer();
  
  tokenType readNextToken();
  string* nextToken();

  
  string* getToken();
  tokenType getTokenType();
  
  int getLineNumber();
  int getPosition();
		
private:
  istream* in;
		
  string currentToken;
  tokenType currentTokenType;
						
  int lineNum, position;
  
  void IDchar();

  bool readIdent();
  bool readName();
  bool readNMStart();
  bool readNonAscii();
  bool readEscape();
  bool readUnicode();
  bool readNMChar();
  bool readNum();
  bool readString();
  bool readNewline();
  bool readWhitespace();
  bool readUrl();
  bool isHex(char c);
  bool readComment();
  bool readUnicodeRange ();
		
protected:
  tokenType lastReadType;
  char lastRead;
};

#endif
