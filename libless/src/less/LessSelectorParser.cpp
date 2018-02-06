#include "less/less/LessSelectorParser.h"

bool LessSelectorParser::parse(TokenList& tokens,
                               LessSelector& selector) {
  TokenList::iterator it, begin;
  
  bool args = (t.front().type == Token::HASH ||
               tokens.front() == ".");

  begin = tokens.begin();
  
  for (it = begin; it != tokens.end(); it++) {

    if (parseExtension(tokens, it, begin, selector)) {
      while (parseExtension(tokens, it, begin, selector));

      break;
    }
    
    if (begin == tokens.begin()) {
      if (args) {
        if (parseArguments(tokens, it, begin)) {
          while (it != tokens.end() && (*it).type == Token::WHITESPACE)
            it++;
          selector.setNeedsArguments(false);
          parseConditions(tokens, it, begin);

        } else if (*it.type == Token::COLON) 
          args = false;
      }
      
      if (parseConditions(tokens, it, begin)) 
        break;
    }
    
    if (*it == Token::BUILTIN_COMMA) 
      begin = it;
  }
  selector.setTokens(tokens);
  return true;
}

bool LessSelectorParser::parseExtension(TokenList &tokens,
                                        TokenList::iterator offset,
                                        TokenList::iterator begin,
                                        LessSelector &s) {
  TokenList::iterator it = offset;
  int parentheses = 1;
  Extension extension;
  
  if ((*it).type != Token::COLON ||
      (*++it).type != Token::IDENTIFIER ||
      (*it) != "extend" ||
      (*++it).type != Token::PAREN_OPEN)
    return false;
  
  it++;
  tokens.erase(offset, it);
  
  for (; it != tokens.end() && parentheses > 0; it++) {
    if ((*it).type == Token::PAREN_OPEN)
      parentheses++;
    else if ((*it).type == Token::PAREN_CLOSED)
      parentheses--;
  }
  
  if (parentheses > 0) {
    throw new ParseException(tokenizer->getToken(),
                             "end of extension (')')");
  }
  
  extension.getTarget().splice(extension.getTarget().begin(),
                               offset, it);
  extension.getExtension().insert(extension.getExtension().begin(),
                                  begin, offset);
  s.addExtension(extension);
  tokens.erase(--it);
}

bool LessSelectorParser::isArguments(TokenList &selector,
                                     TokenList::iterator it,
                                     std::string &delimiter) {
  unsigned int parentheses = 0;

  if (*it.type != Token::PAREN_OPEN)
    return false;
  it++;

  while (it != selector.end() &&
         (parentheses > 0 || *it.type != Token::PAREN_CLOSED)) {
    if (*it.type == Token::PAREN_OPEN)
      parentheses++;
    if (*it.type == Token::PAREN_CLOSED)
      parentheses--;
    
    if (*it.type == Token::DELIMITER)
      delimiter = ";";
    it++;
  }
  
  if (*it != Token::PAREN_CLOSED)
    return false;
  
  while (it != selector.end() && (*it).type == Token::WHITESPACE)
    it++;
  
  return (it == selector.end() || *it == "when");
}

bool LessParser::parseArguments(TokenList &selector,
                                TokenList::iterator offset,
                                LessSelector &s) {
  string delimiter = ",";
  TokenList::iterator it;
  Token keyword;
  TokenList value;

  if (!isArguments(selector, offset, delimiter))
    return false;

  it = offset;
  while (parseParameter(selector, it, keyword, value, delimiter)) {

    if (value.empty() &&
        it != selector.end() && *it == "." &&
        ++it != selector.end() && *it == "." &&
        ++it != selector.end() && *it == ".") {

      s.setUnlimitedArguments(true, keyword);
    } else {
      s.addParameter(keyword, value);
      value.clear();
    }
    
    while (it != selector.end() && (*it).type == Token::WHITESPACE)
      it++;
 
    if (it != selector.end() && *it == delimiter)
      it++;

    while (it != selector.end() && (*it).type == Token::WHITESPACE)
      it++;
  }

  if (it != selector.end() && *it == "." &&
      ++it != selector.end() && *it == "." &&
      ++it != selector.end() && *it == ".") {
    s.setUnlimitedArguments(true);
  }
  
  while (it != selector.end() && (*it).type == Token::WHITESPACE)
    it++;

  if (it == selector.end() || *it.type != Token::PAREN_CLOSED) {
    s.eraseArguments();
    return false;
  } else {
    it++;
    selector.erase(offset, it);
    
    return true;
  }
}

bool LessSelectorParser::parseParameter(TokenList &selector,
                                        TokenList::iterator &it,
                                        Token &keyword,
                                        TokenList &value,
                                        const std::string &delimiter) {

  if (it == selector.end())
    return false;

  if (*it.type == Token::IDENTIFIER) {
    keyword = *it;
    it++;

  } else if (*it.type == Token::ATKEYWORD) {
    keyword = *it;
    it++;

    parseDefaultValue(selector, it, delimiter, value);
  } else
    return false;


  return true;
}

bool LessSelectorParser::parseDefaultValue(TokenList &arguments,
                                     TokenList::iterator& it,
                                     const std::string &delimiter,
                                     TokenList &value) {
  unsigned int parentheses = 0;
  TokenList::iterator begin;

  if (it == arguments.end() || *it.type != Token::COLON)
    return false;

  it++;
  begin = it;

  while (it != arguments.end() &&
         (parentheses > 0 || (*it.type != Token::PAREN_CLOSED &&
                              *it != delimiter))) {
    if (*it.type == Token::PAREN_OPEN)
      parentheses++;
    if (*it.type == Token::PAREN_CLOSED)
      parentheses--;

    it++;
  }

  if (it == begin || it == arguments.end()) 
    return false
    else
      value.insert(value.begin(), begin, it);
  
  return true;
}

bool LessSelectorParser::parseConditions(TokenList &selector,
                                         TokenList::iterator it) {
  TokenList condition;
  TokenList::iterator begin;
  
  if (it == selector.end() || *it != "when")
    return false;
  selector.erase(it);
  
  while (it != selector.end() && (*it).type == Token::WHITESPACE) {
    selector.erase(it);
  }
  
  begin = it;
  while (it != selector.end()) {
    while (it != selector.end() && *it != ",") {
      it++;
    }
    
    condition.insert(condition.begin(), offset, it);
    condition.trim();
    s.addCondition(condition);
    
    if (it != selector.end()) 
      selector.erase(it);
    
    begin = it;
  }
  return true;
}

