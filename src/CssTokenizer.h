#ifndef __CssTokenizer_h__
#define __CssTokenizer_h__

#include <ctype.h>
#include <string>
#include <iostream>
#include "IOException.h"
#include "ParseException.h"

using namespace std;


/**
 * Converts the input stream to CSS tokens according to the syntax spec at
 * http://www.w3.org/TR/CSS2/syndata.html.
 *
 * Excerpt (replace '(forward-slash)' with '/'):
 *
 *   IDENT      	{ident}
 *   ATKEYWORD  	@{ident}
 *   STRING     	{string}
 *   BAD_STRING 	{badstring}
 *   BAD_URI    	{baduri}
 *   BAD_COMMENT 	{badcomment}
 *   HASH       	#{name}
 *   NUMBER     	{num}
 *   PERCENTAGE 	{num}%
 *   DIMENSION  	{num}{ident}
 *   URI        	url\({w}{string}{w}\)
 *                      |url\({w}([!#$%&*-\[\]-~]|{nonascii}|{escape})*{w}\)
 *   UNICODE-RANGE 	u\+[0-9a-f?]{1,6}(-[0-9a-f]{1,6})?
 *   CDO        	<!--
 *   CDC        	-->
 *   :          	:
 *   ;          	;
 *   {          	\{
 *   }          	\}
 *   (          	\(
 *   )          	\)
 *   [          	\[
 *   ]          	\]
 *   S          	[ \t\r\n\f]+
 *   COMMENT     	\/\*[^*]*\*+([^(forward-slash)*][^*]*\*+)*\/
 *   FUNCTION   	{ident}\(
 *   INCLUDES    	~=
 *   DASHMATCH   	|=
 *   DELIM      	any other character not matched by the above
 *                      rules, and neither a single nor a double quote 
 *
 * Notes
 *
 * The CDO and CDC tokens are not supported as they are not expected in a
 * css file.
 *
 * Instead of generating BAD_STRING, BAD_URI and BAD_COMMENT tokens
 * the CssTokenizer throws a ParseException. This may change in the
 * future so the Parser can handle them.
 *
 * FUNCTION tokens are not generated. If an identifier is followed by a '(' the
 * parser will handle it from there.
 *
 * The macros in curly braces ({}) above are defined as follows:
 *   Macro      	Definition
 *   ident      	[-]?{nmstart}{nmchar}*
 *   name       	{nmchar}+
 *   nmstart    	[_a-z]|{nonascii}|{escape}
 *   nonascii   	[^\0-\237]
 *   unicode    	\\[0-9a-f]{1,6}(\r\n|[ \n\r\t\f])?
 *   escape     	{unicode}|\\[^\n\r\f0-9a-f]
 *   nmchar     	[_a-z0-9-]|{nonascii}|{escape}
 *   num        	[0-9]+|[0-9]*\.[0-9]+
 *   string     	{string1}|{string2}
 *   string1    	\"([^\n\r\f\\"]|\\{nl}|{escape})*\"
 *   string2    	\'([^\n\r\f\\']|\\{nl}|{escape})*\'
 *   badstring   	{badstring1}|{badstring2}
 *   badstring1 	\"([^\n\r\f\\"]|\\{nl}|{escape})*\\?
 *   badstring2 	\'([^\n\r\f\\']|\\{nl}|{escape})*\\?
 *   badcomment 	{badcomment1}|{badcomment2}
 *   badcomment1 	\/\*[^*]*\*+([^(forward-slash)*][^*]*\*+)*
 *   badcomment2 	\/\*[^*]*(\*+[^(forward-slash)*][^*]*)*
 *   baduri     	{baduri1}|{baduri2}|{baduri3}
 *   baduri1    	url\({w}([!#$%&*-~]|{nonascii}|{escape})*{w}
 *   baduri2    	url\({w}{string}{w}
 *   baduri3    	url\({w}{badstring}
 *   nl         	\n|\r\n|\r|\f
 *   w          	[ \t\r\n\f]*
 */
class CssTokenizer {
public:
  enum tokenType{IDENTIFIER, ATKEYWORD, STRING, HASH, NUMBER, PERCENTAGE,
                 DIMENSION, URL, UNICODE_RANGE, COLON, DELIMITER, BRACKET_OPEN,
                 BRACKET_CLOSED, PAREN_OPEN, PAREN_CLOSED, BRACE_OPEN,
                 BRACE_CLOSED, WHITESPACE, COMMENT, INCLUDES, DASHMATCH, OTHER,
                 EOS}; 
	
  CssTokenizer(istream* in);
		
  ~CssTokenizer();
  
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
