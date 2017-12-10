#include "less/css/CssPrettyWriter.h"

void CssPrettyWriter::indent() {
  int i;
  if (indent_size == 0)
    return;
  for (i = 0; i < indent_size; i++)
    writeStr("  ", 2);
}

void CssPrettyWriter::newline() {
  out->write("\n", 1);
  column = 0;
  
  if (sourcemap != NULL)
    sourcemap->writeNewline();
}

void CssPrettyWriter::writeSelector(const TokenList &selector) {
  TokenList::const_iterator it;

  if (sourcemap != NULL)
    sourcemap->writeMapping(column, selector.front());
  
  for (it = selector.begin(); it != selector.end(); it++) {
    writeToken(*it);
      
    if ((*it) == ",") {
      if (sourcemap != NULL)
        sourcemap->writeMapping(column, selector.front());

      newline();
      indent();
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

void CssPrettyWriter::writeComment(const Token &comment) {
  writeToken(comment);
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
