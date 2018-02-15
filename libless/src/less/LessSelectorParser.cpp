#include "less/less/LessSelectorParser.h"
#include "less/css/ParseException.h"
#include "less/lessstylesheet/Extension.h"


bool LessSelectorParser::parse(TokenList& tokens,
                               LessSelector& selector) {
  std::list<TokenList>::iterator it;
  TokenList::iterator offset;
  
  bool args = (tokens.front().type == Token::HASH ||
               tokens.front() == ".");

  if (tokens.contains(Token::IDENTIFIER, "when")) {
    selector.push_back(tokens);
  } else {
    CssSelectorParser::parse(tokens, selector);
  }

  for (it = selector.begin();
       it != selector.end();
       it++) {

    for (offset = (*it).begin(); offset != (*it).end(); offset++) {
      
      if (parseExtension(*it, offset, selector)) {
        while (parseExtension(*it, offset, selector));
        break;
      }

      if (selector.size() == 1) {
        
        if (args) {
          if (parseArguments(*it, offset, selector)) {
            while (offset != (*it).end() && (*offset).type == Token::WHITESPACE)
              offset++;
            
            selector.setNeedsArguments(true);
            parseConditions(*it, offset, selector);
            
          } else if ((*offset).type == Token::COLON) 
            args = false;
        }

        parseConditions(*it, offset, selector);
      }
    }
    (*it).trim();
  }

  return true;
}

bool LessSelectorParser::parseExtension(TokenList &tokens,
                                        TokenList::iterator &offset,
                                        LessSelector &s) {
  TokenList::iterator it = offset;
  int parentheses = 1;
  Extension extension;
  TokenList target, ext;
  
  if (it == tokens.end() ||
      (*it).type != Token::COLON ||
      ++it == tokens.end() ||
      (*it).type != Token::IDENTIFIER ||
      (*it) != "extend" ||
      ++it == tokens.end() ||
      (*it).type != Token::PAREN_OPEN)
    return false;
  
  it++;
  tokens.erase(offset, it);
  offset = it;
  
  for (; it != tokens.end() && parentheses > 0; it++) {
    if ((*it).type == Token::PAREN_OPEN)
      parentheses++;
    else if ((*it).type == Token::PAREN_CLOSED)
      parentheses--;
  }
  
  if (parentheses > 0) {
    throw new ParseException(*offset,
                             "end of extension (')')");
  }
  it--;
  
  target.splice(target.begin(), tokens, offset, it);
  offset = it;
  ext.insert(ext.begin(), tokens.begin(), offset);
    
  if (!target.empty() && target.back() == "all") {
    extension.setAll(true);
    target.pop_back();
    target.rtrim();
  }
  selectorParser.parse(target, extension.getTarget());
  selectorParser.parse(ext, extension.getExtension());
  s.addExtension(extension);

  offset++;
  tokens.erase(it);
  return true;
}

bool LessSelectorParser::isArguments(TokenList &selector,
                                     TokenList::iterator it,
                                     std::string &delimiter) {
  unsigned int parentheses = 0;

  if ((*it).type != Token::PAREN_OPEN)
    return false;
  it++;

  while (it != selector.end() &&
         (parentheses > 0 || (*it).type != Token::PAREN_CLOSED)) {
    if ((*it).type == Token::PAREN_OPEN)
      parentheses++;
    if ((*it).type == Token::PAREN_CLOSED)
      parentheses--;
    
    if ((*it).type == Token::DELIMITER)
      delimiter = ";";
    it++;
  }
  
  if ((*it).type != Token::PAREN_CLOSED)
    return false;
  it++;
  
  while (it != selector.end() && (*it).type == Token::WHITESPACE)
    it++;
  
  return (it == selector.end() || *it == "when");
}

bool LessSelectorParser::parseArguments(TokenList &selector,
                                TokenList::iterator &offset,
                                LessSelector &s) {
  std::string delimiter = ",";
  TokenList::iterator it;
  Token keyword;
  TokenList value;

  if (!isArguments(selector, offset, delimiter))
    return false;

  it = offset;
  it++;
  while (parseParameter(selector, it, keyword, value, delimiter)) {
    if (value.empty() &&
        it != selector.end() && *it == "." &&
        ++it != selector.end() && *it == "." &&
        ++it != selector.end() && *it == ".") {

      s.setUnlimitedArguments(true, keyword);
      it++;
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
    it++;
  }
  
  while (it != selector.end() && (*it).type == Token::WHITESPACE)
    it++;

  if (it == selector.end() || (*it).type != Token::PAREN_CLOSED) {
    s.eraseArguments();
    return false;
  } else {
    it++;
    selector.erase(offset, it);
    offset = it;
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

  if ((*it).type == Token::IDENTIFIER) {
    keyword = *it;
    it++;

  } else if ((*it).type == Token::ATKEYWORD) {
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

  if (it == arguments.end() || (*it).type != Token::COLON)
    return false;

  it++;
  begin = it;

  while (it != arguments.end() &&
         (parentheses > 0 || ((*it).type != Token::PAREN_CLOSED &&
                              *it != delimiter))) {
    if ((*it).type == Token::PAREN_OPEN)
      parentheses++;
    if ((*it).type == Token::PAREN_CLOSED)
      parentheses--;

    it++;
  }

  if (it == begin || it == arguments.end()) 
    return false;
  else {
    value.insert(value.begin(), begin, it);
    value.trim();
  }
  return true;
}

bool LessSelectorParser::parseConditions(TokenList &selector,
                                         TokenList::iterator &offset,
                                         LessSelector &s) {
  TokenList condition;
  TokenList::iterator it = offset;
  
  if (it == selector.end() || *it != "when")
    return false;
  it = selector.erase(it);
  offset = it;
  
  while (it != selector.end()) {
    while (it != selector.end() && *it != ",") {
      it++;
    }
    
    condition.splice(condition.begin(), selector, offset, it);
    condition.trim();
    s.addCondition(condition);
    condition.clear();
    
    if (it != selector.end()) 
      it = selector.erase(it);
    offset = it;
  }
  return true;
}

