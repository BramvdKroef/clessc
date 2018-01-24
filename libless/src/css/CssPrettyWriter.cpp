#include "less/css/CssPrettyWriter.h"

void CssPrettyWriter::indent() {
  int i;
  if (indent_size == 0)
    return;
  for (i = 0; i < indent_size; i++)
    writeStr("  ", 2);
}


void CssPrettyWriter::writeSelector(const TokenList &selector) {
  TokenList::const_iterator it;
  bool newselector = true;
  
  for (it = selector.begin(); it != selector.end(); it++) {
    if (newselector && sourcemap != NULL) {
      sourcemap->writeMapping(column, *it);
      newselector = false;
    }

    writeToken(*it);

    if ((*it) == ",") {
      newline();
      
      indent();
      newselector = true;
    }
  }
}

void CssPrettyWriter::writeAtRule(const Token &keyword, const TokenList &rule) {
  indent();

  CssWriter::writeAtRule(keyword, rule);
  newline();
}

void CssPrettyWriter::writeRulesetStart(const TokenList &selector) {
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
  indent();

  writeSelector(selector);

  writeStr(" {", 2);
  newline();
  indent_size++;
}

void CssPrettyWriter::writeMediaQueryEnd() {
  writeStr("}", 1);
  indent_size--;
  newline();
}
