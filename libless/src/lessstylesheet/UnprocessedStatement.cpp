#include "less/lessstylesheet/UnprocessedStatement.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/Mixin.h"

UnprocessedStatement::UnprocessedStatement() {
  property_i = 0;
}

Selector *UnprocessedStatement::getTokens() {
  return &tokens;
}

void UnprocessedStatement::getProperty(TokenList &tokens) {
  TokenList::iterator i = this->tokens.begin();
  std::advance(i, property_i);

  tokens.insert(tokens.begin(), this->tokens.begin(), i);
}

void UnprocessedStatement::getValue(TokenList &tokens) {
  TokenList::iterator i = this->tokens.begin();
  std::advance(i, property_i);

  tokens.insert(tokens.begin(), i, this->tokens.end());
}

void UnprocessedStatement::setLessRuleset(LessRuleset &r) {
  lessRuleset = &r;
}

LessRuleset *UnprocessedStatement::getLessRuleset() {
  return lessRuleset;
}

void UnprocessedStatement::process(Stylesheet &s) {
  AtRule *target;
  Mixin mixin;
  mixin.setStylesheet(getLessRuleset()->getLessStylesheet());

  if (getTokens()->front().type == Token::ATKEYWORD) {
    target = s.createAtRule(getTokens()->front());
    getValue(target->getRule());
    getLessRuleset()->getContext()->processValue(target->getRule());

    // process mixin
  } else if (mixin.parse(*getTokens()))
    mixin.call(s, *getLessRuleset()->getContext(), NULL, getLessRuleset());
}

void UnprocessedStatement::process(Ruleset &r) {
  Extension extension;
  Mixin mixin;
  Declaration *declaration;

  if (getTokens()->front().type == Token::ATKEYWORD) {
    // Can't add @-rules to rulesets so ignore the statement.
    return;
  }

  // process extends statement
  if (getExtension(extension.getTarget())) {
    extension.setExtension(r.getSelector());
    getLessRuleset()->getContext()->addExtension(extension);
    return;
  }

  mixin.setStylesheet(getLessRuleset()->getLessStylesheet());

  declaration = r.createDeclaration();

  if (isDeclaration() && processDeclaration(*declaration)) {

    getLessRuleset()->getContext()->interpolate(declaration->getProperty());
    getLessRuleset()->getContext()->processValue(declaration->getValue());

  } else {
    r.deleteDeclaration(*declaration);
    // process mixin
    if (mixin.parse(*getTokens()) && mixin.call(*r.getStylesheet(),
                                                *getLessRuleset()->getContext(),
                                                &r,
                                                getLessRuleset())) {
    } else {
      throw new ParseException(getTokens()->toString(),
                               "variable, mixin or declaration.",
                               getTokens()->front().line,
                               getTokens()->front().column,
                               getTokens()->front().source);
    }
  }

}

bool UnprocessedStatement::isDeclaration() {
  TokenList::iterator i = getTokens()->begin();
  if (property_i == 0)
    return false;

  if ((*i).type == Token::HASH || (*i) == ".")
    return false;

  std::advance(i, property_i);
  while (i != getTokens()->end() && (*i).type == Token::WHITESPACE) {
    i++;
  }
  if (i == getTokens()->end() || (*i).type != Token::COLON) {
    return false;
  }
  return true;
}

bool UnprocessedStatement::isExtends() {
  TokenList::iterator i = getTokens()->begin();

  return ((*i) == "&" && (*++i).type == Token::COLON &&
          (*++i).type == Token::IDENTIFIER && (*i) == "extend" &&
          (*++i).type == Token::PAREN_OPEN);
}

bool UnprocessedStatement::getExtension(TokenList &extension) {
  TokenList::iterator i;
  int parentheses = 1;

  if (!isExtends())
    return false;

  i = getTokens()->begin();

  // &:extends(
  std::advance(i, 4);

  for (; i != getTokens()->end() && parentheses > 0; i++) {
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
      extension.push_back(*i);
  }

  if (parentheses > 0) {
    throw new ParseException("end of statement",
                             ")",
                             getTokens()->front().line,
                             getTokens()->front().column,
                             getTokens()->front().source);
  }
  return true;
}

bool UnprocessedStatement::processDeclaration(Declaration &declaration) {
  TokenList property;
  Token keyword;

  getValue(declaration.getValue());

  // fix: If there's a Token (not empty) and if this token is a space
  if (declaration.getValue().empty() == false &&
      declaration.getValue().front().type == Token::WHITESPACE) {
    // Then we dismiss it to process the next token which should be a colon
    declaration.getValue().pop_front();
  }

  if (declaration.getValue().empty() ||
      declaration.getValue().front().type != Token::COLON) {
    return NULL;
  }

  declaration.getValue().pop_front();

  getProperty(property);
  keyword = property.front();
  keyword.assign(property.toString());

  declaration.setProperty(keyword);

  return true;
}
