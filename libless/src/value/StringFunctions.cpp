#include "less/value/StringFunctions.h"
#include "less/value/FunctionLibrary.h"
#include "less/value/Value.h"
#include "less/value/StringValue.h"
#include "less/value/NumberValue.h"
#include "less/value/Color.h"
#include <regex>

#ifdef _MSC_VER
#if _MSC_VER < 1900 
namespace std
{
	using namespace tr1;
}
#endif
#endif

void StringFunctions::loadFunctions(FunctionLibrary& lib) {
  lib.push("escape", "S", &StringFunctions::escape);
  lib.push("e", "S", &StringFunctions::e);
  lib.push("%", "S.+", &StringFunctions::format);
  lib.push("replace", "SSSS?", &StringFunctions::replace);
  lib.push("color", "S", &StringFunctions::color);
  lib.push("data-uri", "SS?", &StringFunctions::data_uri);
}

Value* StringFunctions::escape(const vector<const Value*>& arguments) {
  string unreservedChars(
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~,/"
                         "?@&+'!$");

  StringValue* s = new StringValue(*(const StringValue*)arguments[0]);
  s->setString(StringValue::escape(s->getString(), ",/?@&+'!$"));
  return s;
}

Value* StringFunctions::e(const vector<const Value*>& arguments) {
  StringValue* s = new StringValue(*(const StringValue*)arguments[0]);
  s->setQuotes(false);
  return s;
}

Value* StringFunctions::format(const vector<const Value*>& arguments) {
  std::string escapeChars("adsADS");

  StringValue* s = new StringValue(*(const StringValue*)arguments[0]);

  std::string oldstr = s->getString();
  std::ostringstream newstr;
  unsigned int i, argc = 1;
  std::string argStr;

  for (i = 0; i < oldstr.size(); i++) {
    if (oldstr[i] == '%') {
      i++;

      if (escapeChars.find(oldstr[i]) != string::npos) {
        if (argc == arguments.size())
          throw new ValueException(
                                   "Format template expects more \
arguments than provided.",
                                   *arguments[0]->getTokens());

        if ((oldstr[i] == 's' || oldstr[i] == 'S') &&
            arguments[argc]->type == Value::STRING) {
          argStr = ((const StringValue*)arguments[argc])->getString();
        } else
          argStr = arguments[argc]->getTokens()->toString();

        if (oldstr[i] == 'A' || oldstr[i] == 'D' || oldstr[i] == 'S') {
          argStr = StringValue::escape(argStr);
        }
        newstr << argStr;
        argc++;
      } else
        newstr << oldstr[i];
    } else
      newstr << oldstr[i];
  }

  if (argc != arguments.size()) {
    throw new ValueException(
                             "Format template does not supply \
placeholders for all given arguments.",
                             *arguments[0]->getTokens());
  }

  s->setString(newstr.str());
  return s;
}

Value* StringFunctions::replace(const vector<const Value*>& arguments) {
  std::string out;
  std::regex regex;
  std::regex_constants::syntax_option_type regex_flags =
    std::regex_constants::ECMAScript;
  std::regex_constants::match_flag_type match_flags =
    std::regex_constants::match_default |
    std::regex_constants::format_first_only;

  const StringValue* in = (const StringValue*)arguments[0];
  const StringValue* pattern = (const StringValue*)arguments[1];
  const StringValue* replacement = (const StringValue*)arguments[2];
  std::string options;

  if (arguments.size() > 3) {
    options = ((const StringValue*)arguments[3])->getString();

    if (options.find('i') != std::string::npos)
      regex_flags |= std::regex::icase;
    if (options.find('g') != std::string::npos)
      match_flags &= ~std::regex_constants::format_first_only;
  }

  regex = std::regex(pattern->getString(), regex_flags);
  out = std::regex_replace(
                           in->getString(), regex, replacement->getString(), match_flags);
  return new StringValue(out, in->getQuotes());
}

Value* StringFunctions::color(const vector<const Value*>& arguments) {
  const StringValue* s;
  Token t;

  s = static_cast<const StringValue*>(arguments[0]);

  t = Token(s->getString(), Token::HASH, 0, 0, "generated");
  return new Color(t);
}
Value* StringFunctions::data_uri(const vector<const Value*>& arguments) {
  return new StringValue(*(const StringValue*)arguments[0]);
}
