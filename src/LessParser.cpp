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

#include "LessParser.h"

bool LessParser::parseStatement(Stylesheet* stylesheet) {
  return parseRuleset(stylesheet) || parseAtRuleOrVariable(stylesheet);
}

bool LessParser::parseAtRuleOrVariable (Stylesheet* stylesheet) {
  string keyword, import;
  TokenList* rule;
  AtRule* atrule = NULL;
  
  if (tokenizer->getTokenType() != Token::ATKEYWORD) 
    return false;

  keyword = tokenizer->getToken()->str;
  tokenizer->readNextToken();
  skipWhitespace();

  if (!parseVariable(keyword)) {
    rule = new TokenList();
    while(parseAny(rule)) {};
  
    if (!parseBlock(rule)) {
      if (tokenizer->getTokenType() != Token::DELIMITER) {
        throw new ParseException(tokenizer->getToken()->str,
                                 "delimiter (';') at end of @-rule");
      }
      tokenizer->readNextToken();
      skipWhitespace();
    }
    // parse import
    if (keyword == "@import") {
      if (rule->size() != 1 ||
          rule->front()->type != Token::STRING)
        throw new ParseException(*rule->toString(), "A string with the \
file path");
      import = rule->front()->str;
      if (import.size() < 5 ||
          import.substr(import.size() - 5, 4) != ".css") {
        if (import.size() < 6 || import.substr(import.size() - 6, 5) != ".less")
          import.insert(import.size() - 1, ".less");
        
        importFile(import.substr(1, import.size() - 2), stylesheet);
        return true;
      }
    } 
    atrule = new AtRule(new string(keyword));
    atrule->setRule(rule);
    stylesheet->addAtRule(atrule);
  }
  return true;
}

bool LessParser::parseVariable (string keyword = "") {
  TokenList* rule;
  
  if (keyword == "") {
    if (tokenizer->getTokenType() != Token::ATKEYWORD) 
      return false;
    keyword = tokenizer->getToken()->str;
    tokenizer->readNextToken();
    skipWhitespace();

    if (tokenizer->getTokenType() != Token::COLON)
      throw new ParseException(tokenizer->getToken()->str,
                               "colon (':') following @keyword in \
variable declaration.");
  } else if (tokenizer->getTokenType() != Token::COLON)
    return false;
  
  tokenizer->readNextToken();
  skipWhitespace();
    
  rule = parseValue();

  if (rule == NULL || rule->size() == 0) {
    throw new ParseException(tokenizer->getToken()->str,
                             "value for variable");
  }
  if (tokenizer->getTokenType() != Token::DELIMITER) {
    throw new ParseException(tokenizer->getToken()->str,
                             "delimiter (';') at end of @-rule");
  }
  valueProcessor->putVariable(keyword, rule);
  tokenizer->readNextToken();
  skipWhitespace();
  return true;
}

bool LessParser::parseRuleset (Stylesheet* stylesheet,
                               Selector* selector,
                               ParameterRuleset* parent) {
  Ruleset* ruleset = NULL;
  ParameterRuleset* pruleset = NULL;
  list<string> parameters;
  list<string>::iterator pit;
  
  if (selector == NULL) {
    selector = new Selector();
    parseSelector(selector);
  }
  
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
    if (selector->empty()) {
      delete selector;
      return false;
      
    } else if (parseParameterMixin(selector, NULL, stylesheet, NULL)) {
      // Parameter mixin in the root. Inserts nested rules but no
      // declarations. 
      delete selector;
      if (tokenizer->getTokenType() == Token::DELIMITER) {
        tokenizer->readNextToken();
        skipWhitespace();
      }
      return true;
    } else {
      throw new ParseException(tokenizer->getToken()->str,
                               "a declaration block ('{...}')");
    }
  }
  tokenizer->readNextToken();
  skipWhitespace();

  // Create the ruleset and parse ruleset statements.
  // In case of a parameter ruleset the declaration values are not
  // processed until later.
  if (ParameterRuleset::isValid(selector)) {
    if (parent != NULL)
      cerr << "Warning: Parametric ruleset defined inside another \
parametric ruleset." << endl;
    ruleset = pruleset = new ParameterRuleset(selector);
    pRulesets->addRule(pruleset);
    while (parseRulesetStatement(stylesheet, ruleset, pruleset)) {
    }
  } else {
    ruleset = new Ruleset(selector);
    if (parent == NULL)
      stylesheet->addRuleset(ruleset);
    else
      parent->addNestedRule(ruleset);

    // add new scope for the ruleset
    valueProcessor->pushScope();
    while (parseRulesetStatement(stylesheet, ruleset, parent)) {
    }
    valueProcessor->popScope();
  }    
  
  if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
    throw new ParseException(tokenizer->getToken()->str,
                             "end of declaration block ('}')");
  } 
  tokenizer->readNextToken();
  skipWhitespace();
  
  return true;
}

bool LessParser::parseRulesetStatement (Stylesheet* stylesheet,
                                        Ruleset* ruleset,
                                        ParameterRuleset* parent) {
  Declaration* declaration;
  TokenList property;
  Selector* selector = new Selector();

  parseProperty(&property);
  parseWhitespace(selector);
  parseSelector(selector);

  if (!property.empty()) {
    selector->unshift(&property);
  }
  selector->trim();
  
  // No selector found. Try to parse a variable and return.
  if (selector->empty())  {
    delete selector;
    return parseVariable();
  }

  // a selector followed by a ruleset is a nested rule
  if (parseNestedRule(selector, ruleset, stylesheet, parent)) {
    
    // a selector by itself might be a mixin.
  } else if ((tokenizer->getTokenType() == Token::DELIMITER ||
             tokenizer->getTokenType() == Token::BRACKET_CLOSED) &&
             parseMixin(selector, ruleset, stylesheet, parent)) {
    delete selector;

  } else {
    for(size_t i=0; i < property.size(); i++)
      delete selector->shift();
    
    if ((declaration = parseDeclaration(&property, selector)) != NULL) {
      if (parent == NULL)
        valueProcessor->processValue(declaration->getValue());

      ruleset->addDeclaration(declaration);
    
    } else {
      throw new ParseException(*selector->toString(),
                               "a mixin, nested ruleset, or property");
    }
  }
  if (tokenizer->getTokenType() == Token::DELIMITER) {
    tokenizer->readNextToken();
    skipWhitespace();
  }
    
  return true;
}


bool LessParser::parseNestedRule(Selector* selector,
                                 Ruleset* ruleset,
                                 Stylesheet* stylesheet,
                                 ParameterRuleset* parent) {
  
  if (tokenizer->getTokenType() != Token::BRACKET_OPEN)
    return false;

  if (ruleset != parent)
    selector->addPrefix(ruleset->getSelector());
  parseRuleset(stylesheet, selector, parent);
  return true;
}

Declaration* LessParser::parseDeclaration (TokenList* property,
                                           TokenList* value) {
  Declaration* declaration;
  TokenList* value2;
  
  if (value->empty() ||
      value->front()->type != Token::COLON)
    return NULL;
    
  delete value->shift();
  
  declaration = new Declaration(property->toString());
  
  // parse any leftover value parts.
  value2 = parseValue();
  if (value2 != NULL) {
    value->push(value2);
    delete value2;
  }
  declaration->setValue(value);
  
  return declaration;
}

bool LessParser::parseMixin(Selector* selector, Ruleset* ruleset,
                            Stylesheet* stylesheet,
                            ParameterRuleset* parent) {
  Ruleset* mixin;
  vector<Declaration*>* declarations;
  bool ret;

  ret = parseParameterMixin(selector, ruleset, stylesheet, parent);

  if ((mixin = stylesheet->getRuleset(selector)) != NULL) {
    declarations = mixin->cloneDeclarations();
    ruleset->addDeclarations(declarations);
    delete declarations;
    return true;
  } 
  return ret;
}

bool LessParser::parseParameterMixin(Selector* selector,
                                     Ruleset* target,
                                     Stylesheet* stylesheet,
                                     ParameterRuleset* parent) {
  ParameterMixin* mixin = new ParameterMixin(new Selector(),
                                             new list<TokenList*>(),
                                             new Selector());
  TokenList* argument;  
  TokenListIterator* tli = selector->iterator();
  Token* current;
  size_t nestedParenthesis = 0;
  bool ret = false;

  while (tli->hasNext()) {
    current = tli->next();
    if (current->type == Token::PAREN_OPEN)
      break;
    mixin->name->push(current->clone());
  }

  // delete trailing whitespace
  mixin->name->trim();

  while (tli->hasNext()) {
    argument = new TokenList();

    while (tli->hasNext()) {
      current = tli->next();
      if (nestedParenthesis == 0) {
        if (current->str == "," ||
            current->str == ";" ||
            current->type == Token::PAREN_CLOSED)
          break;
      }
      
      if (current->type == Token::PAREN_OPEN) 
        nestedParenthesis++;
      
      if (current->type == Token::PAREN_CLOSED) 
        nestedParenthesis--;
      
      argument->push(current->clone());
    }

    if (parent == NULL)
      valueProcessor->processValue(argument);
    mixin->arguments->push_back(argument);
  }
  
  delete tli;
  if (!pRulesets->parameterRulesetExists(mixin)) {
    delete mixin;
    return false;
  }
  
  if (target != NULL && target != parent)
    mixin->prefix->push(target->getSelector());

  if (parent != NULL) {
    parent->addMixin(mixin);
    return true;
  } else {
    ret = pRulesets->processParameterMixin(mixin, target, stylesheet);
    delete mixin;
    return ret;
  }
}


void LessParser::importFile(string filename, Stylesheet* stylesheet) {
  ifstream* in = new ifstream(filename.c_str());
  if (in->fail() || in->bad())
    throw new ParseException(filename, "existing file");

  LessTokenizer* tokenizer = new LessTokenizer(in);
  LessParser* parser = new LessParser(tokenizer,
                              this->pRulesets,
                              this->valueProcessor);
  parser->parseStylesheet(stylesheet);
  in->close();
  delete parser;
  delete tokenizer;
  delete in;
}
