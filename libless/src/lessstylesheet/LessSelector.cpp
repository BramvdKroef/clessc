#include "less/LogStream.h"
#include "less/lessstylesheet/LessRuleset.h"

LessSelector::LessSelector(const Selector &original) {
  list<Selector> parts;
  list<Selector>::iterator it;
  Selector *old_selector;
  Selector new_selector;

  original.split(parts);

  _needsArguments = false;
  _unlimitedArguments = false;

  LogStream().notice(2) << "Parsing less selector";

  for (it = parts.begin(); it != parts.end(); it++) {
    old_selector = &(*it);

    while (!old_selector->empty()) {
      if (parseExtension(*old_selector, new_selector)) {
      } else if (parts.size() == 1 && !new_selector.empty()) {
        if ((new_selector.front().type == Token::HASH ||
             new_selector.front() == ".") &&
            parseArguments(*old_selector)) {
          _needsArguments = true;
          old_selector->ltrim();

        } else if (!parseConditions(*old_selector)) {
          new_selector.push_back(old_selector->front());
          old_selector->pop_front();
        }
      } else {
        new_selector.push_back(old_selector->front());
        old_selector->pop_front();
      }
    }
    if (!empty())
      push_back(Token::BUILTIN_COMMA);

    new_selector.trim();
    insert(end(), new_selector.begin(), new_selector.end());
    new_selector.clear();
  }

  LogStream().notice(2) << "Parsed selector: " << toString();
}

LessSelector::~LessSelector() {
}

bool LessSelector::parseExtension(Selector &selector, Selector &extension) {
  int parentheses = 1;
  TokenList::iterator i;
  Extension e;

  i = selector.begin();

  // ":", "extend", "("
  if (selector.size() < 3 || (*i).type != Token::COLON ||
      (*++i).type != Token::IDENTIFIER || (*i) != "extend" ||
      (*++i).type != Token::PAREN_OPEN)
    return false;

  i++;

  for (; i != selector.end() && parentheses > 0; i++) {
    if ((*i).type == Token::PAREN_OPEN)
      parentheses++;
    else if ((*i).type == Token::PAREN_CLOSED)
      parentheses--;

    if (parentheses > 0) {
      e.getTarget().push_back(*i);
    }
  }

  e.setExtension(extension);
  extensions.push_back(e);

  selector.erase(selector.begin(), i);

  LogStream().notice(2) << "Extension: " << extension.toString();

  return true;
}

bool LessSelector::parseArguments(TokenList &selector) {
  string delimiter;
  TokenList::iterator i;

  if (selector.front().type != Token::PAREN_OPEN)
    return false;

  if (selector.contains(Token::DELIMITER, ";"))
    delimiter = ";";
  else
    delimiter = ",";

  LogStream().notice(3) << "Parameter delimiter: " << delimiter;

  if (!validateArguments(selector, delimiter))
    return false;

  selector.pop_front();

  selector.ltrim();

  while (parseParameter(selector, delimiter)) {
    selector.ltrim();
  }

  i = selector.begin();

  if (selector.size() > 3 && (*i) == "." && (*++i) == "." && (*++i) == ".") {
    _unlimitedArguments = true;
    i++;
    selector.erase(selector.begin(), i);
  }

  selector.ltrim();

  if (selector.front().type != Token::PAREN_CLOSED) {
    throw new ParseException(
        selector.toString(), "matching parentheses.", 0, 0, "");
  }
  selector.pop_front();

  LogStream().notice(3) << "Done parsing parameters";

  return true;
}

bool LessSelector::validateArguments(const TokenList &arguments,
                                     const std::string &delimiter) {
  TokenList::const_iterator i = arguments.begin();

  if ((*i).type != Token::PAREN_OPEN)
    return false;

  i++;

  while (i != arguments.end() && (*i).type == Token::WHITESPACE) {
    i++;
  }

  while (i != arguments.end()) {
    if ((*i).type == Token::IDENTIFIER) {
      // switch
      i++;

    } else if ((*i).type == Token::ATKEYWORD) {
      // variable
      i++;

      if ((*i).type == Token::COLON) {
        // default value
        i++;
        while (i != arguments.end() && (*i).type != Token::PAREN_CLOSED &&
               *i != delimiter) {
          i++;
        }

      } else if (*i == ".") {
        i++;
        // rest
        if (i == arguments.end() || *i != "." || ++i == arguments.end() ||
            *i != ".") {
          return false;
        }
        i++;
        break;
      }
    } else
      break;

    if (*i != delimiter)
      break;
    i++;

    while (i != arguments.end() && (*i).type == Token::WHITESPACE) {
      i++;
    }
  }

  while (i != arguments.end() && (*i).type == Token::WHITESPACE) {
    i++;
  }

  // rest
  if (*i == ".") {
    i++;
    if (i == arguments.end() || *i != "." || ++i == arguments.end() ||
        *i != ".") {
      return false;
    }
    i++;
  }
  if ((*i).type != Token::PAREN_CLOSED)
    return false;

  LogStream().notice(2) << "Validated parameters";

  return true;
}

bool LessSelector::parseParameter(TokenList &selector,
                                  const std::string &delimiter) {
  string keyword;
  TokenList value;
  TokenList::iterator i;

  if (selector.empty())
    return false;

  if (selector.front().type == Token::IDENTIFIER) {
    keyword = selector.front();
    selector.pop_front();

  } else if (selector.front().type == Token::ATKEYWORD) {
    keyword = selector.front();
    selector.pop_front();

    i = selector.begin();

    if (parseDefaultValue(selector, delimiter, value)) {
      // default value

    } else if (selector.size() > 3 && (*i) == "." && (*++i) == "." &&
               (*++i) == ".") {
      i++;
      // rest argument
      selector.erase(selector.begin(), i);

      restIdentifier = keyword;
      _unlimitedArguments = true;
      return true;
    }
  } else
    return false;

  selector.ltrim();

  if (!selector.empty() && selector.front() == delimiter)
    selector.pop_front();

  LogStream().notice(2) << "Parameter: " << keyword
                        << " default: " << value.toString();

  parameters.push_back(keyword);
  defaults.push_back(value);
  return true;
}

bool LessSelector::parseDefaultValue(TokenList &arguments,
                                     const std::string &delimiter,
                                     TokenList &value) {
  unsigned int parentheses = 0;

  if (arguments.front().type != Token::COLON)
    return false;

  arguments.pop_front();

  while (!arguments.empty() &&
         (parentheses > 0 || (arguments.front().type != Token::PAREN_CLOSED &&
                              arguments.front() != delimiter))) {
    if (arguments.front().type == Token::PAREN_OPEN)
      parentheses++;
    if (arguments.front().type == Token::PAREN_CLOSED)
      parentheses--;

    value.push_back(arguments.front());
    arguments.pop_front();
  }

  value.trim();

  if (value.empty()) {
    throw new ParseException("", "default value following ':'", 0, 0, "");
  }
  return true;
}

bool LessSelector::parseConditions(TokenList &selector) {
  TokenList condition;

  if (selector.empty() || selector.front() != "when")
    return false;

  LogStream().notice(3) << "Parsing conditions";

  selector.pop_front();
  selector.ltrim();

  while (!selector.empty()) {
    while (!selector.empty() && selector.front() != ",") {
      condition.push_back(selector.front());
      selector.pop_front();
    }
    if (!selector.empty() && selector.front() == ",")
      selector.pop_front();

    LogStream().notice(2) << "Condition: " << condition.toString();

    conditions.push_back(condition);
  }
  return true;
}

TokenList *LessSelector::getDefault(const std::string &keyword) {
  std::list<std::string>::iterator pit = parameters.begin();
  std::list<TokenList>::iterator dit = defaults.begin();

  for (; pit != parameters.end(); pit++, dit++) {
    if ((*pit) == keyword)
      return &(*dit);
  }
  return NULL;
}

std::list<std::string> &LessSelector::getParameters() {
  return parameters;
}

std::list<TokenList> &LessSelector::getConditions() {
  return conditions;
}

std::list<Extension> &LessSelector::getExtensions() {
  return extensions;
}

bool LessSelector::matchArguments(const Mixin &mixin) {
  std::list<std::string>::iterator p_it = parameters.begin();
  std::list<TokenList>::iterator d_it = defaults.begin();
  size_t pos = 0;

  for (; p_it != parameters.end(); p_it++, d_it++) {
    if (mixin.getArgument(*p_it) == NULL && mixin.getArgument(pos++) == NULL &&
        (*d_it).empty()) {
      return false;
    }
  }
  return (pos >= mixin.getArgumentCount() || unlimitedArguments());
}

bool LessSelector::needsArguments() {
  return _needsArguments;
}
bool LessSelector::unlimitedArguments() {
  return _unlimitedArguments;
}
string LessSelector::getRestIdentifier() {
  return restIdentifier;
}
