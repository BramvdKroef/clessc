#include "UnprocessedStatement.h"

UnprocessedStatement::UnprocessedStatement() {
  property_i = 0;
  type = UNPROCESSED;
}

Selector* UnprocessedStatement::getTokens(){
  return &tokens;
}

void UnprocessedStatement::getProperty(TokenList* tokens) {
  for(size_t i=0; i < property_i; i++) {
    tokens->push(this->tokens.at(i));
  }
}

void UnprocessedStatement::getValue(TokenList* tokens) {
  for(size_t i=property_i; i < this->tokens.size(); i++) {
    tokens->push(this->tokens.at(i));
  }
}

int UnprocessedStatement::getType() {
  return type;
}


bool UnprocessedStatement::processVariable(ValueProcessor* valueProcessor) {
  string keyword;
  
  if (getTokens()->front()->type == Token::ATKEYWORD &&
      getTokens()->at(1)->type == Token::COLON) {
    keyword = getTokens()->front()->str;
    delete getTokens()->shift();
    
    valueProcessor->putVariable(keyword, getTokens());
    return true;
  } else
    return false;
}

bool UnprocessedStatement::processDeclaration (Declaration* declaration) {
  TokenList property;
  TokenList* value = new TokenList();

  getValue(value);
  
  if (value->empty() ||
      value->front()->type != Token::COLON) {
    delete value;
    return NULL;
  }
    
  delete value->shift();
  
  getProperty(&property);
  
  declaration->setProperty(property.toString());
  declaration->setValue(value);
  
  return declaration;
}

RulesetStatement* UnprocessedStatement::clone() {
  UnprocessedStatement* statement = new UnprocessedStatement();
  statement->line = line;
  statement->column = column;
  statement->property_i = property_i;
  statement->getTokens()->push(getTokens());
  return statement;
}
