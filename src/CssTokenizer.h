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

#ifndef __CssTokenizer_h__
#define __CssTokenizer_h__

#include <iostream>
#include <string>
#include "Token.h"
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
	
  CssTokenizer(istream &in, const string &source);
		
  ~CssTokenizer();
  
  Token::Type readNextToken();
  
  Token getToken();
  Token::Type getTokenType();

  unsigned int getLineNumber();
  unsigned int getColumn();
  string getSource();
		
protected:
  istream* in;

  Token currentToken;
  char lastRead;
  
  unsigned int line, column;
  string source;
  
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
  bool readUnicodeRange ();

  bool lastReadEq(char c);
  bool lastReadInRange(char c1, char c2);
  bool lastReadIsDigit();
  bool lastReadIsHex();
};

#endif
