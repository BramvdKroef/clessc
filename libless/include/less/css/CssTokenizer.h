#ifndef __less_css_CssTokenizer_h__
#define __less_css_CssTokenizer_h__

#include <iostream>
#include <string>
#include "less/Token.h"
#include "less/css/IOException.h"
#include "less/css/ParseException.h"

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
 * The CDO and CDC tokens(html comments) are not supported as they are
 * not expected in a css file.
 *
 * Instead of generating BAD_STRING, BAD_URI and BAD_COMMENT tokens
 * the CssTokenizer throws a ParseException. This may change in the
 * future so the Parser can handle them.
 *
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
  CssTokenizer(istream& in, const char* source);

  virtual ~CssTokenizer();

  Token::Type readNextToken();

  Token& getToken();
  Token::Type getTokenType();

  const char* getSource();

protected:
  istream* in;

  Token currentToken;
  char lastRead;

  unsigned int line, column;
  const char* source;

  void readChar();

  bool readIdent();
  bool readName();
  bool readNMStart();
  bool readNonAscii();
  bool readEscape();
  bool readUnicode();
  bool readNMChar();
  bool readNum(bool readDecimals);
  bool readNumSuffix();
  bool readString();
  bool readNewline();
  bool readWhitespace();
  bool readUrl();
  virtual bool readComment();
  bool readUnicodeRange();

  bool lastReadEq(char c);
  bool lastReadInRange(char c1, char c2);
  bool lastReadIsDigit();
  bool lastReadIsHex();
};

#endif  // __less_css_CssTokenizer_h__
