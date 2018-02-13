#include "less/css/CssPrettyWriter.h"

void CssPrettyWriter::indent() {
  int i;
  if (indent_size == 0)
    return;
  for (i = 0; i < indent_size; i++)
    writeStr("  ", 2);
}


void CssPrettyWriter::writeSelector(const Selector &selector) {
  std::list<TokenList>::const_iterator s_it;
  TokenList::const_iterator token;

  for(s_it = selector.begin();
      s_it != selector.end();
      s_it++) {

    if (s_it != selector.begin()) {
      writeStr(",", 1);
      newline();
      indent();
    }
    
    for (token = (*s_it).begin(); token != (*s_it).end(); token++) {
      if (sourcemap != NULL && token == (*s_it).begin()) 
        sourcemap->writeMapping(column, *token);
      
      writeToken(*token);
    }
  }
}

void CssPrettyWriter::writeAtRule(const Token &keyword, const TokenList &rule) {
  indent();

  CssWriter::writeAtRule(keyword, rule);
  newline();
}

void CssPrettyWriter::writeRulesetStart(const Selector &selector) {
  indent();

  writeSelector(selector);
  writeStr(" {", 2);
  newline();
  indent_size++;
}

void CssPrettyWriter::writeRulesetEnd() {
  writeStr(";", 1);
  newline();

  indent_size--;
  indent();

  writeStr("}", 1);
  newline();
}
void CssPrettyWriter::writeDeclaration(const Token &property,
                                       const TokenList &value) {
  indent();

  if (sourcemap != NULL)
    sourcemap->writeMapping(column, property);

  writeToken(property);
  writeStr(": ", 2);

  writeValue(value);
}

void CssPrettyWriter::writeDeclarationDeliminator() {
  writeStr(";", 1);
  newline();
}

void CssPrettyWriter::writeMediaQueryStart(const TokenList &selector) {
  TokenList::const_iterator token;

  indent();

  for (token = selector.begin(); token != selector.end(); token++) {
    if (sourcemap != NULL && token == selector.begin()) 
      sourcemap->writeMapping(column, *token);
      
    writeToken(*token);
  }

  writeStr(" {", 2);
  newline();
  indent_size++;
}

void CssPrettyWriter::writeMediaQueryEnd() {
  writeStr("}", 1);
  indent_size--;
  newline();
}
