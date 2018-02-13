#ifndef __less_css_CssPrettyWriter_h__
#define __less_css_CssPrettyWriter_h__

#include <iostream>
#include "less/Token.h"
#include "less/TokenList.h"
#include "less/css/CssWriter.h"
#include "less/stylesheet/Selector.h"

class CssPrettyWriter : public CssWriter {
private:
  int indent_size;

protected:
  void indent();

  virtual void writeSelector(const Selector &selector);

public:
  CssPrettyWriter(std::ostream &out) : CssWriter(out) {
    indent_size = 0;
  };
  CssPrettyWriter(std::ostream &out, SourceMapWriter &sourcemap)
      : CssWriter(out, sourcemap) {
    indent_size = 0;
  }

  virtual void writeAtRule(const Token &keyword, const TokenList &rule);
  virtual void writeRulesetStart(const Selector &selector);
  virtual void writeRulesetEnd();
  virtual void writeDeclaration(const Token &property, const TokenList &value);
  virtual void writeDeclarationDeliminator();
  virtual void writeMediaQueryStart(const TokenList &selector);
  virtual void writeMediaQueryEnd();
};

#endif  // __less_css_CssPrettyWriter_h__
