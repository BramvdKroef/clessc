#include "UnprocessedStatement.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

UnprocessedStatement::UnprocessedStatement() {
  property_i = 0;
  type = UNPROCESSED;
}

Selector* UnprocessedStatement::getTokens(){
  return &tokens;
}

void UnprocessedStatement::getProperty(TokenList* tokens) {
  for(size_t i=0; i < property_i; i++) {
    tokens->push(this->tokens.at(i)->clone());
  }
}

void UnprocessedStatement::getValue(TokenList* tokens) {
  for(size_t i=property_i; i < this->tokens.size(); i++) {
    tokens->push(this->tokens.at(i)->clone());
  }
}

int UnprocessedStatement::getType() {
  return type;
}


bool UnprocessedStatement::processDeclaration (Declaration* declaration) {
  TokenList property;
  TokenList* value = new TokenList();

  DLOG(INFO) << "Declaration";
  
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
