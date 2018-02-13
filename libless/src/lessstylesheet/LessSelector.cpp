#include "less/lessstylesheet/LessRuleset.h"

LessSelector::LessSelector(): Selector() {
  _unlimitedArguments = false;
  _needsArguments = false;
}
LessSelector::~LessSelector() {
}


const TokenList *LessSelector::getDefault(const std::string &keyword) const {
  std::list<std::string>::const_iterator pit = parameters.begin();
  std::list<TokenList>::const_iterator dit = defaults.begin();

  for (; pit != parameters.end(); pit++, dit++) {
    if ((*pit) == keyword)
      return &(*dit);
  }
  return NULL;
}

const std::list<std::string> &LessSelector::getParameters() const {
  return parameters;
}

const std::list<TokenList> &LessSelector::getConditions() const {
  return conditions;
}

void LessSelector::addExtension(Extension &extension) {
  extensions.push_back(extension);
}
const std::list<Extension> &LessSelector::getExtensions() const {
  return extensions;
}

void LessSelector::addParameter(Token &keyword, TokenList &value) {
  parameters.push_back(keyword);
  defaults.push_back(value);
}

void LessSelector::setUnlimitedArguments(bool b) {
  _unlimitedArguments = b;
}
void LessSelector::setUnlimitedArguments(bool b, Token restKeyword) {
  _unlimitedArguments = b;
  restIdentifier = restKeyword;
}

void LessSelector::setNeedsArguments(bool b) {
  _needsArguments = b;
}

void LessSelector::eraseArguments() {
  parameters.clear();
  defaults.clear();
  _unlimitedArguments = false;
  restIdentifier = "";
  _needsArguments = false;
}

void LessSelector::addCondition(TokenList &condition) {
  conditions.push_back(condition);
}

bool LessSelector::matchArguments(const MixinArguments &args) const {
  std::list<std::string>::const_iterator p_it = parameters.begin();
  std::list<TokenList>::const_iterator d_it = defaults.begin();
  size_t pos = 0;
  const TokenList *a;

  for (; p_it != parameters.end(); p_it++, d_it++) {
    if ((a = args.get(*p_it)) == NULL &&
        (a = args.get(pos++)) == NULL &&
        (a = &(*d_it))->empty()) {
      return false;
    } else if ((*p_it)[0] != '@' &&
               a->size() == 1 &&
               (*p_it) != a->front()) {
      return false;
    }
  }
  return (pos >= args.count() || unlimitedArguments());
}

bool LessSelector::needsArguments() const {
  return _needsArguments;
}
bool LessSelector::unlimitedArguments() const {
  return _unlimitedArguments;
}
string LessSelector::getRestIdentifier() const {
  return restIdentifier;
}

