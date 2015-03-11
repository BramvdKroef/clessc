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

#ifndef __Token_h__
#define __Token_h__

#include <string>

class Token: public std::string {
public:
  enum Type{IDENTIFIER, ATKEYWORD, STRING, HASH, NUMBER, PERCENTAGE,
            DIMENSION, URL, UNICODE_RANGE, COLON, DELIMITER, BRACKET_OPEN,
            BRACKET_CLOSED, PAREN_OPEN, PAREN_CLOSED, BRACE_OPEN,
            BRACE_CLOSED, WHITESPACE, COMMENT, INCLUDES,
            DASHMATCH, OTHER, EOS} type; 
  //  std::string str;
  
  Token ();
  
  Token (const std::string &s, Type t);

  /**
   * Clear the characters in the token and set the type to OTHER.
   */
  void clear ();

  /**
   * Returns true if the token string starts with a " or a '.
   */
  bool stringHasQuotes() const;
  
  /**
   * Removes quotes from STRING tokens.
   */
  void removeQuotes();

  /**
   * Removes quotes from given parameter str.
   */
  void removeQuotes(std::string &str) const;

  /**
   * Returns the value from URL tokens with quotes removed.
   *
   * For example Token 'url("abc.css")' returns 'abc.css'
   */
  std::string getUrlString() const;
  
  inline std::string& append(char c) {
    return std::string::append(1, c);
  }
  inline std::string& append(const std::string &c) {
    return std::string::append(c);
  }

  inline bool operator == (const Token &t) const {
    return (type == t.type &&
            (std::string)*this == (std::string)t);
  }
  inline bool operator != (const Token &t) const {
    return !(*this == t);
  }
  
  inline bool operator < (const Token &t) const {
    return (type == t.type) ?
      (std::string)*this < (std::string)t : 
      (type < t.type);
  }

  inline bool operator > (const Token &t) const {
    return t < *this;
  }
  inline bool operator <= (const Token &t) const {
    return !(t < *this);
  }
  inline bool operator >= (const Token &t) const {
    return !(*this < t);
  }

  inline Token& operator= (const std::string& str) {
    std::string::assign(str);
    return *this;
  }
};

#endif
