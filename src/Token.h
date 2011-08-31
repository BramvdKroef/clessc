#ifndef __Token_h__
#define __Token_h__

#include <string>
using namespace std;

class Token {
public:
  enum Type{IDENTIFIER, ATKEYWORD, STRING, HASH, NUMBER, PERCENTAGE,
            DIMENSION, URL, UNICODE_RANGE, COLON, DELIMITER, BRACKET_OPEN,
            BRACKET_CLOSED, PAREN_OPEN, PAREN_CLOSED, BRACE_OPEN,
            BRACE_CLOSED, WHITESPACE, COMMENT, FUNCTION, INCLUDES,
            DASHMATCH, OTHER, EOS} type; 
  string str;

  Token () {
    type = OTHER;
  }
  Token (string s, Type t) {
    str = s;
    type = t;
  }
  
  void add(char c) {
    str.append(1, c);
  }
  
  void clear () {
    str.clear();
    type = OTHER;
  }
  Token* clone () {
    return new Token(str, type);
  }
};

#endif
