#include "less/less/LessParser.h"


/**
 * Only allows LessStylesheets
 */
void LessParser::parseStylesheet(LessStylesheet &stylesheet) {
  CssParser::parseStylesheet(stylesheet);
}

void LessParser::parseStylesheet(LessRuleset &ruleset) {
  while (parseRulesetStatement(ruleset));
}

void LessParser::skipWhitespace() {
  while (tokenizer->getTokenType() == Token::WHITESPACE ||
         (tokenizer->getTokenType() == Token::COMMENT &&
          tokenizer->getToken().compare(0, 2, "//") == 0)) {
    tokenizer->readNextToken();
  }
}

bool LessParser::parseStatement(Stylesheet &stylesheet) {
  TokenList selector;
  CssComment *comment;
  LessStylesheet *ls = (LessStylesheet *)&stylesheet;

  if (tokenizer->getTokenType() == Token::COMMENT) {
    comment = ls->createComment();
    comment->setComment(tokenizer->getToken());
    tokenizer->readNextToken();
    skipWhitespace();
    return true;

  } else if (parseSelector(selector) && !selector.empty()) {
    if (parseRuleset(*ls, selector))
      return true;

    // Parameter mixin in the root. Inserts nested rules but no
    // declarations.
    if (parseMixin(selector, *ls)) {
      if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokenizer->readNextToken();
        skipWhitespace();
      }
      return true;
    } else {
      throw new ParseException(
          tokenizer->getToken(),
          "a declaration block ('{...}') following selector");
    }

  } else {
    return parseAtRuleOrVariable(*ls);
  }
}

bool LessParser::parseAtRuleOrVariable(LessStylesheet *stylesheet,
                                       LessRuleset *ruleset) {
  Token token;
  TokenList value, rule;
  AtRule *atrule = NULL;

  if (tokenizer->getTokenType() != Token::ATKEYWORD)
    return false;

  token = tokenizer->getToken();
  tokenizer->readNextToken();
  CssParser::skipWhitespace();

  if (parseVariable(value)) {
    if (stylesheet != NULL)
      stylesheet->putVariable(token, value);
    else
      ruleset->putVariable(token, value);

  } else if (token == "@media") {
    if (stylesheet != NULL)
      parseLessMediaQuery(token, *stylesheet);
    else
      parseMediaQueryRuleset(token, *ruleset);

  } else {

    parseAtRuleValue(rule);

    // parse import
    if (token == "@import" && rule.size() > 0) {
      if (parseImportStatement(rule, stylesheet, ruleset))
        return true;
    }

    if (stylesheet != NULL)
      atrule = stylesheet->createLessAtRule(token);
    else
      atrule = ruleset->createLessAtRule(token);

    atrule->setReference(reference);
    atrule->setRule(rule);
  }
  return true;
}

bool LessParser::parseAtRuleOrVariable(LessStylesheet &stylesheet) {
  return parseAtRuleOrVariable(&stylesheet, NULL);
}

bool LessParser::parseAtRuleOrVariable(LessRuleset &ruleset) {
  return parseAtRuleOrVariable(NULL, &ruleset);
}


bool LessParser::parseAtRuleValue(TokenList &rule) {
  while (parseAny(rule)) {
  };

  if (!parseBlock(rule)) {
    if (tokenizer->getTokenType() != Token::DELIMITER) {
      throw new ParseException(tokenizer->getToken(),
                               "delimiter (';') at end of @-rule");
    }
    tokenizer->readNextToken();
    skipWhitespace();
  }
  return true;
}

bool LessParser::parseVariable(TokenList &value) {
  if (tokenizer->getTokenType() != Token::COLON)
    return false;

  tokenizer->readNextToken();
  CssParser::skipWhitespace();

  if (parseValue(value) == false || value.size() == 0) {
    throw new ParseException(tokenizer->getToken(), "value for variable");
  }
  if (tokenizer->getTokenType() != Token::DELIMITER) {
    throw new ParseException(tokenizer->getToken(),
                             "delimiter (';') at end of @-rule");
  }
  tokenizer->readNextToken();
  skipWhitespace();

  return true;
}

bool LessParser::parseSelector(TokenList &selector) {
  if (!parseAny(selector))
    return false;

  while (parseAny(selector) || parseSelectorVariable(selector)) {
  };

  // delete trailing whitespace
  selector.rtrim();
  return true;
}

bool LessParser::parseSelectorVariable(TokenList &selector) {
  Token *back;

  if (tokenizer->getTokenType() == Token::BRACKET_OPEN) {
    back = &selector.back();

    if (back->at(back->length() - 1) == '@') {
      back->append(tokenizer->getToken());

      if (tokenizer->readNextToken() != Token::IDENTIFIER)
        throw new ParseException(tokenizer->getToken(),
                                 "Variable inside selector (e.g.: \
@{identifier})");
      back->append(tokenizer->getToken());

      if (tokenizer->readNextToken() != Token::BRACKET_CLOSED)
        throw new ParseException(tokenizer->getToken(),
                                 "Closing bracket after variable.");

      back->append(tokenizer->getToken());
      tokenizer->readNextToken();

      parseWhitespace(selector);

      return true;
    }
  }
  return false;
}

bool LessParser::parseRuleset(TokenList &selector,
                              LessStylesheet *stylesheet,
                              LessRuleset *parentRuleset) {
  LessRuleset *ruleset;
  LessSelector *s;

  if (tokenizer->getTokenType() != Token::BRACKET_OPEN)
    return false;

  tokenizer->readNextToken();
  skipWhitespace();

  s = new LessSelector();
  lessSelectorParser.parse(selector, *s);
  
  // Create the ruleset and parse ruleset statements.
  if (parentRuleset == NULL)
    ruleset = stylesheet->createLessRuleset(*s);
  else
    ruleset = parentRuleset->createNestedRule(*s);
  ruleset->setReference(reference);

  while (parseRulesetStatement(*ruleset));

  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of declaration block ('}')");
  }
  tokenizer->readNextToken();
  skipWhitespace();
  
  return true;
}

bool LessParser::parseRuleset(LessStylesheet &parent, TokenList &selector) {
  return parseRuleset(selector, &parent, NULL);
}
bool LessParser::parseRuleset(LessRuleset &parent, TokenList &selector) {
  return parseRuleset(selector, NULL, &parent);
}  

void LessParser::parseMediaQueryRuleset(Token &mediatoken,
                                        LessRuleset &parent) {
  MediaQueryRuleset *query;
  TokenList selector;

  selector.push_back(mediatoken);
  selector.push_back(Token::BUILTIN_SPACE);

  CssParser::skipWhitespace();

  while (parseAny(selector) || tokenizer->getTokenType() == Token::ATKEYWORD) {
    if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      selector.push_back(tokenizer->getToken());
      tokenizer->readNextToken();
      parseWhitespace(selector);
    }
  }

  query = parent.createMediaQuery(selector);
  query->setReference(reference);
  
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken(), "{");
  }
  tokenizer->readNextToken();
  skipWhitespace();

  while (parseRulesetStatement(*query));

  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of media query block ('}')");
  }
  tokenizer->readNextToken();
  skipWhitespace();
}

bool LessParser::parsePropertyVariable(TokenList &selector) {
  Token variable = tokenizer->getToken();

  if (tokenizer->getTokenType() != Token::OTHER || variable != "@")
    return false;

  if (tokenizer->readNextToken() != Token::BRACKET_OPEN)
    throw new ParseException(tokenizer->getToken(),
                             "Opening bracket following @");
  variable.append(tokenizer->getToken());

  if (tokenizer->readNextToken() != Token::IDENTIFIER)
    throw new ParseException(tokenizer->getToken(),
                             "Variable inside selector (e.g.: \
@{identifier})");

  variable.append(tokenizer->getToken());

  if (tokenizer->readNextToken() != Token::BRACKET_CLOSED)
    throw new ParseException(tokenizer->getToken(),
                             "Closing bracket after variable.");

  variable.append(tokenizer->getToken());
  tokenizer->readNextToken();

  selector.push_back(variable);
  parseWhitespace(selector);

  return true;
}

bool LessParser::parseRulesetStatement(LessRuleset &ruleset) {
  TokenList tokens;
  size_t property_i;

  if (parseComment(ruleset))
    return true;
  
  if (parseAtRuleOrVariable(ruleset))
    return true;
  
  while (parseProperty(tokens) || parsePropertyVariable(tokens)) {
  }

  // merge properties have a '+' or '+_' suffix
  if (tokenizer->getToken() == "+") {
    tokens.push_back(tokenizer->getToken());
    tokenizer->readNextToken();
    if (tokenizer->getToken() == "_") {
      tokens.push_back(tokenizer->getToken());
      tokenizer->readNextToken();
    }
  }

  property_i = tokens.size();

  parseWhitespace(tokens);
  if (tokenizer->getTokenType() != Token::COLON)
    property_i = 0;
  
  parseSelector(tokens);
  tokens.trim();

  if (tokens.empty())
    return NULL;

  if (parseRuleset(ruleset, tokens))
    return true;

  parseValue(tokens);
  
  if (parseExtension(tokens, ruleset) ||
      parseDeclaration(tokens, property_i, ruleset)) {
  } else {
    parseMixin(tokens, ruleset);
  }

  if (tokenizer->getTokenType() == Token::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
  }
  return true;
}

bool LessParser::parseComment(LessRuleset& ruleset) {
  CssComment *comment;
  if (tokenizer->getTokenType() != Token::COMMENT) 
    return false;
  
  comment = ruleset.createComment();
  comment->setComment(tokenizer->getToken());
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

bool LessParser::parseExtension(TokenList &statement, LessRuleset &ruleset) {
  TokenList::iterator i = statement.begin();
  int parentheses = 1;
  Extension extension;
  TokenList target;

  if (statement.size() <= 5 ||
      (*i) != "&" ||
      (*++i).type != Token::COLON ||
      (*++i).type != Token::IDENTIFIER ||
      (*i) != "extend" ||
      (*++i).type != Token::PAREN_OPEN)
    return false;

  i++;
  for (; i != statement.end() && parentheses > 0; i++) {
    switch ((*i).type) {
    case Token::PAREN_OPEN:
      parentheses++;
      break;
    case Token::PAREN_CLOSED:
      parentheses--;
      break;
    default:
      break;
    }
    if (parentheses > 0)
      target.push_back(*i);
  }

  if (parentheses > 0) {
    throw new ParseException("end of statement",
                             ")",
                             statement.front().line,
                             statement.front().column,
                             statement.front().source);
  }

  if (!target.empty() && target.back() == "all") {
    extension.setAll(true);
    target.pop_back();
    target.rtrim();
  }
  selectorParser.parse(target, extension.getTarget());

  ruleset.addExtension(extension);
  
  return true;
}

bool LessParser::parseDeclaration(TokenList &tokens,
                                  size_t property_i,
                                  LessRuleset &ruleset) {
  LessDeclaration* d;
  TokenList::iterator i;
  TokenList property;
  Token keyword;
  
  if (property_i == 0 ||
      tokens.front().type == Token::HASH ||
      tokens.front() == ".") {
    return false;
  }
  
  d = ruleset.createLessDeclaration();

  i = tokens.begin();
  std::advance(i, property_i);
  
  property.insert(property.begin(), tokens.begin(), i);
  keyword = property.front();
  keyword.assign(property.toString());
  d->setProperty(keyword);

  while (i != tokens.end() && (*i).type == Token::WHITESPACE) 
    i++;
  
  if (i != tokens.end() && (*i).type == Token::COLON)
    i++;
  
  while (i != tokens.end() && (*i).type == Token::WHITESPACE) 
    i++;
  
  d->getValue().insert(d->getValue().begin(), i, tokens.end());

  return true;
}

bool LessParser::parseMixin(TokenList &tokens,
                            LessRuleset &parent) {
  return parseMixin(tokens, &parent, NULL);
}

bool LessParser::parseMixin(TokenList &tokens,
                            LessStylesheet &parent) {
  return parseMixin(tokens, NULL, &parent);
}

bool LessParser::parseMixin(TokenList &tokens,
                            LessRuleset *parent_r,
                            LessStylesheet *parent_s) {
  TokenList::const_iterator i = tokens.begin();
  Mixin *mixin;
  TokenList name;
  
  for (; i != tokens.end() && (*i).type != Token::PAREN_OPEN; i++) {
    name.push_back(*i);
  }

  name.rtrim();
  if (parent_r != NULL)
    mixin = parent_r->createMixin(name);
  else
    mixin = parent_s->createMixin(name);
  mixin->setReference(reference);
  parseMixinArguments(i, tokens, *mixin);

  while(i != tokens.end() && (*i).type == Token::WHITESPACE)
    i++;

  if (i != tokens.end() && *i == "!") {
    if (++i != tokens.end() && *i == "important") {
      mixin->setImportant(true);
      i++;
    } else
      i--;
  }

  if (i != tokens.end())
    throw new ParseException(*i, "end of mixin statement");
  return true;
}

void LessParser::parseMixinArguments(TokenList::const_iterator &i,
                                     const TokenList &tokens,
                                     Mixin &mixin) {
  TokenList::const_iterator j;
  std::string delimiter = ",";

  TokenList argument;
  size_t nestedParenthesis = 0;
  std::string argName;

  if (i != tokens.end() && (*i).type == Token::PAREN_OPEN) {
    i++;
  }

  // if a ';' token occurs then that is the delimiter instead of the ','.
  if (tokens.contains(Token::DELIMITER, ";")) {
    delimiter = ";";
  }

  while (i != tokens.end() && (*i).type != Token::PAREN_CLOSED) {
    while (i != tokens.end() && (*i).type == Token::WHITESPACE) {
      i++;
    }

    // atkeyword followed by comma is an argument name
    if ((*i).type == Token::ATKEYWORD) {
      argName = (*i);
      i++;
      if (i != tokens.end() && (*i).type == Token::COLON) {
        i++;
      } else {
        argName = "";
        i--;
      }
    }
    // parse until delimiter, or end of argument list.
    while (i != tokens.end() &&
           (nestedParenthesis > 0 ||
            ((*i) != delimiter && (*i).type != Token::PAREN_CLOSED))) {
      if ((*i).type == Token::PAREN_OPEN)
        nestedParenthesis++;

      if ((*i).type == Token::PAREN_CLOSED)
        nestedParenthesis--;

      argument.push_back(*i);

      i++;
    }

    if (*i == delimiter)
      i++;

    if (argName == "")
      mixin.arguments.add(argument);
    else {
      mixin.arguments.add(argName, argument);
      argName = "";
    }
    argument.clear();
  }
  
  if (i != tokens.end())
    i++;
}

bool LessParser::parseImportStatement(TokenList &statement,
                                      LessStylesheet *stylesheet,
                                      LessRuleset *ruleset) {
  unsigned int directive = 0;

  // parse directives and strip from statement (the statement becomes a valid
  // css import statement.)
  if (statement.size() >= 4 && statement.front().type == Token::PAREN_OPEN) {
    statement.pop_front();
    statement.ltrim();

    directive = parseImportDirective(statement.front());
    statement.pop_front();
    statement.ltrim();

    while (statement.size() > 0 && statement.front() == ",") {
      statement.pop_front();
      statement.ltrim();

      directive |= parseImportDirective(statement.front());
      statement.pop_front();
      statement.ltrim();
    }

    if (statement.size() > 0 && statement.front().type != Token::PAREN_CLOSED) {
      throw new ParseException(statement, ")");
    } else {
      statement.pop_front();
      statement.ltrim();
    }
  }

  if (statement.size() > 0 && (statement.front().type == Token::URL ||
                               statement.front().type == Token::STRING)) {
    return importFile(statement.front(), stylesheet, ruleset, directive);

  } else
    throw new ParseException(statement,
                             "A string with the file path, "
                             "or an import directive.");
}

bool LessParser::parseImportStatement(TokenList &statement,
                                      LessStylesheet &stylesheet) {
  return parseImportStatement(statement, &stylesheet, NULL);
}

bool LessParser::parseImportStatement(TokenList &statement,
                                      LessRuleset &ruleset) {
  return parseImportStatement(statement, NULL, &ruleset);
}

unsigned int LessParser::parseImportDirective(Token &t) {
  if (t.type != Token::IDENTIFIER)
    throw new ParseException(t, "an import directive.");
  if (t == "reference")
    return IMPORT_REFERENCE;
  else if (t == "inline")
    return IMPORT_INLINE;
  else if (t == "less")
    return IMPORT_LESS;
  else if (t == "css")
    return IMPORT_CSS;
  else if (t == "once")
    return IMPORT_ONCE;
  else if (t == "multiple")
    return IMPORT_MULTIPLE;
  else if (t == "optional")
    return IMPORT_OPTIONAL;
  else
    throw new ParseException(t,
                             "valid import directive: reference, "
                             "inline, less, css, once, multiple or optional");
}

bool LessParser::importFile(Token uri,
                            LessStylesheet *stylesheet,
                            LessRuleset *ruleset,
                            unsigned int directive) {
  size_t pathend;
  size_t extension_pos;
  std::list<const char *>::iterator i;
  std::string relative_filename;
  char *relative_filename_cpy;
  std::string extension;

  if (uri.type == Token::URL) {
    uri = uri.getUrlString();

  } else if (uri.type == Token::STRING) {
    uri.removeQuotes();
  }

  // no remote includes
  if (uri.compare(0, 7, "http://") == 0 || uri.compare(0, 8, "https://") == 0)
    return false;

  pathend = uri.rfind('?');
  if (pathend == std::string::npos)
    pathend = uri.size();

  extension_pos = uri.rfind('.', pathend);
  if (extension_pos == std::string::npos) {
    uri.insert(pathend, ".less");
    pathend += 5;
    extension = "less";
  } else
    extension = uri.substr(extension_pos + 1, pathend);

  // don't import css, unless specified with directive
  // don't import if css directive is given
  if ((extension == "css" && !(directive & IMPORT_LESS)) ||
      (directive & IMPORT_CSS)) {
    return false;
  }

  if (!findFile(uri, relative_filename)) {
    if (directive & IMPORT_OPTIONAL)
      return true;
    else {
      throw new ParseException(
          uri, "existing file", uri.line, uri.column, uri.source);
    }
  }

  if (!(directive & IMPORT_MULTIPLE)) {
    // check if the file has already been imported.
    for (i = sources.begin(); i != sources.end(); i++) {
      if (relative_filename == (*i))
        return true;
    }
  }

  ifstream in(relative_filename.c_str());

  relative_filename_cpy = new char[relative_filename.length() + 1];
  std::strcpy(relative_filename_cpy, relative_filename.c_str());

  sources.push_back(relative_filename_cpy);
  LessTokenizer tokenizer(in, relative_filename_cpy);
  LessParser parser(tokenizer, sources, (directive & IMPORT_REFERENCE));

  parser.includePaths = includePaths;

  if (stylesheet != NULL)
    parser.parseStylesheet(*stylesheet);
  else
    parser.parseStylesheet(*ruleset);
  in.close();
  return true;
}

bool LessParser::importFile(Token uri,
                            LessStylesheet &stylesheet,
                            unsigned int directive) {
  return importFile(uri, &stylesheet, NULL, directive);
}
bool LessParser::importFile(Token uri,
                            LessRuleset &ruleset,
                            unsigned int directive) {
  return importFile(uri, NULL, &ruleset, directive);
}

bool LessParser::findFile(Token &uri, std::string &filename) {
  ifstream *in;
  size_t pos;
  std::string source;
  std::list<const char *>::iterator i;

  source = uri.source;
  pos = source.find_last_of("/\\");

  // if the current stylesheet is outside of the current working
  //  directory then add the directory to the filename.
  if (pos != std::string::npos) {
    filename.append(source.substr(0, pos + 1));
  }
  filename.append(uri);

  in = new ifstream(filename.c_str());
  if (in->good()) {
    in->close();
    return true;
  }

  if (includePaths != NULL) {
    for (i = includePaths->begin(); i != includePaths->end(); i++) {
      filename.clear();

      filename.append((*i));
      filename.append(uri);

      in = new ifstream(filename.c_str());
      if (in->good()) {
        in->close();
        return true;
      }
    }
  }
  return false;
}

void LessParser::parseLessMediaQuery(Token &mediatoken,
                                     LessStylesheet &stylesheet) {
  LessMediaQuery *query;
  TokenList selector;
  
  selector.push_back(mediatoken);
  selector.push_back(Token::BUILTIN_SPACE);

  CssParser::skipWhitespace();

  while (parseAny(selector) ||
         tokenizer->getTokenType() == Token::ATKEYWORD) {
    if (tokenizer->getTokenType() == Token::ATKEYWORD) {
      selector.push_back(tokenizer->getToken());
      tokenizer->readNextToken();
      parseWhitespace(selector);
    }
  }

  query = stylesheet.createLessMediaQuery(selector);
  query->setReference(reference);

  if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken(), "{");
  }
  tokenizer->readNextToken();

  skipWhitespace();
  while (parseStatement(*query)) {
    skipWhitespace();
  }

  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken(),
                             "end of media query block ('}')");
  }
  tokenizer->readNextToken();
  skipWhitespace();
}
