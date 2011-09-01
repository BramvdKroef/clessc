#include "LessParser.h"

bool LessParser::parseStatement(Stylesheet* stylesheet) {
  Ruleset* ruleset = parseRuleset();
  if (ruleset != NULL) {
    stylesheet->addRuleset(ruleset);
    return true;
  }
  
  return parseAtRuleOrVariable(stylesheet);
}

bool LessParser::parseAtRuleOrVariable (Stylesheet* stylesheet) {
  string* keyword = NULL;
  vector<Token*>* rule;
  AtRule* atrule = NULL;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return false;

  keyword = new string(tokenizer->getToken()->str);
  tokenizer->readNextToken();
  skipWhitespace();

  if (tokenizer->getTokenType() == Token::COLON) {
    tokenizer->readNextToken();
    skipWhitespace();
    
    rule = parseValue();
    if (rule == NULL) {
      throw new ParseException(tokenizer->getToken()->str,
                               "value for variable");
    }
    if (tokenizer->getTokenType() != Token::DELIMITER) {
      throw new ParseException(tokenizer->getToken()->str,
                               "delimiter (';') at end of @-rule");
    }
    variables[*(keyword)] = rule;
    tokenizer->readNextToken();
    skipWhitespace();
    
  } else {
    rule = new vector<Token*>();
    while(parseAny(rule)) {};
  
    if (!parseBlock(rule)) {
      if (tokenizer->getTokenType() != Token::DELIMITER) {
        throw new ParseException(tokenizer->getToken()->str,
                                 "delimiter (';') at end of @-rule");
      }
      tokenizer->readNextToken();
      skipWhitespace();
    }
    atrule = new AtRule(keyword);
    atrule->setRule(rule);
    stylesheet->addAtRule(atrule);
  }
  return true;
}

Ruleset* LessParser::parseRuleset () {
  Ruleset* ruleset = NULL;
  vector<Token*>* selector = parseSelector();
  
  if (selector == NULL) {
    if (tokenizer->getTokenType() != Token::BRACKET_OPEN) 
      return NULL;
  } else if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    throw new ParseException(tokenizer->getToken()->str,
                             "a declaration block ('{...}')");
  }
  tokenizer->readNextToken();

  ruleset = new Ruleset();
  ruleset->setSelector(selector);

  skipWhitespace();
  parseRulesetStatement(ruleset);
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of declaration block ('}')");
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return ruleset;
}

bool LessParser::parseRulesetStatement (Ruleset* ruleset) {
  Declaration* declaration = NULL;
  vector<Token*>* selector = NULL;
  string* property = parseProperty();
    
  // if we can parse a property and the next token is a COLON then the
  // statement is a declaration.
  if (property != NULL) {
    skipWhitespace();
    if (tokenizer->getTokenType() == Token::COLON) {
      tokenizer->readNextToken();
      skipWhitespace();
      
      declaration = new Declaration(property);
      
      vector<Token*>* value = parseValue();
      if (value == NULL) {
        throw new ParseException(tokenizer->getToken()->str,
                                 "value for property");
      }
      declaration->setValue(value);
      ruleset->addDeclaration(declaration);

      if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokenizer->readNextToken();
        skipWhitespace();
        parseRulesetStatement(ruleset);
      }
      return true;
    }
  }
  
  // otherwise parse a selector
  selector = parseSelector();
  if (selector == NULL && property == NULL)
    return false;

  // Insert property at the front
  if (selector == NULL)
    selector = new vector<Token*>();
  if (property != NULL) {
    selector->insert(selector->begin(),
                     new Token(*property, Token::IDENTIFIER));
    delete property;
  }

  // if followed by a ruleset it's a nested rule, otherwise it's a
  // mixin.
  if (tokenizer->getTokenType() == Token::BRACKET_OPEN) {
    Ruleset* nested = parseRuleset();
    processNestedRuleset(ruleset, nested);
    parseRulesetStatement(ruleset);
  } else {
    /* Ruleset* mixin = stylesheet->getRuleset(selector);*/
    // if (mixin == NULL)
    //  throw new ParseException(tokenizer->getToken()->str,
    //               "value for variable");
    // processMixin(ruleset, mixin);
    if (tokenizer->getTokenType() == Token::DELIMITER) {
      tokenizer->readNextToken();
      skipWhitespace();
      parseRulesetStatement(ruleset);
    }
  }
  return true;
}

vector<Token*>* LessParser::parseValue () {
  vector<Token*>* value = CssParser::parseValue();
  if (value != NULL) {
    vector<Token*>* newvalue = processValue(value);
    delete value;
    return newvalue;
  } else
    return value;
}

vector<Token*>* LessParser::processValue(vector<Token*>* value) {
  vector<Token*>* newvalue = new vector<Token*>();
  vector<Token*>::iterator it;
  Token* token = NULL;
  
  for(it = value->begin(); it < value->end(); it++) {
    token = *it;
    
    if (processVariable(token, newvalue)) {
    } else if (processDeepVariable(token, (*(it + 1)), newvalue)) { 
      it++;
    } else
      newvalue->push_back(token->clone());
  }
  return newvalue;
}

bool LessParser::processVariable (Token* token, vector<Token*>* newvalue) {
  vector<Token*>* var;
  vector<Token*>::iterator it;

  if (token->type == Token::ATKEYWORD && variables.count(token->str)) {
    var = variables[token->str];
    for (it = var->begin(); it < var->end(); it++) 
      newvalue->push_back((*it)->clone());

    return true;
  } else
    return false;
}

bool LessParser::processDeepVariable (Token* token, Token* nexttoken,
                                      vector<Token*>* newvalue) {
  vector<Token*>* var;
  vector<Token*>::iterator it;
  string key("@");
  
  if (token->type == Token::OTHER &&
      token->str == "@" &&
      nexttoken->type == Token::ATKEYWORD &&
      variables.count(nexttoken->str)) {
      
    var = variables[nexttoken->str];
    if (var->size() == 1 && var->front()->type == Token::STRING) {
      key.append(var->front()->
                 str.substr(1, var->front()->str.size() - 2));
      if (variables.count(key)) {
        
        var = variables[key];
        for (it = var->begin(); it < var->end(); it++) 
          newvalue->push_back((*it)->clone());

        return true;
      }
    }
  }
  return false;
}

void LessParser::processNestedRuleset (Ruleset* parent,
                                       Ruleset* nested) {
  vector<Token*>* selector = parent->getSelector();
  vector<Token*>* nSelector = nested->getSelector();
  vector<Token*>::iterator it;
  for (it = selector->begin(); it < selector->end(); it++) {
    nSelector->insert(nSelector->begin(), (*it)->clone());
  }
}
void LessParser::processMixin(Ruleset* parent, Ruleset* mixin) {
  vector<Declaration*>* declarations = mixin->getDeclarations();
  vector<Declaration*>::iterator it;

  for (it = declarations->begin(); it < declarations->end(); it++) {
    parent->addDeclaration((*it)->clone());
  }
}
