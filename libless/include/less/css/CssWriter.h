
#ifndef __CssWriter_h__
#define __CssWriter_h__

#include "../TokenList.h"
#include "SourceMapWriter.h"
#include <iostream>
#include <cstring>

class CssWriter {
protected:
  std::ostream* out;
  unsigned int column;
  SourceMapWriter* sourcemap;

  void writeStr(const char* str, size_t len);
  void writeToken(const Token &token);
  void writeTokenList(const TokenList &tokens);
  
  virtual void writeSelector(const TokenList &selector);
  virtual void writeValue(const TokenList &value);
  
public:
  CssWriter();
  CssWriter(std::ostream &out);
  CssWriter(std::ostream &out,
            SourceMapWriter &sourcemap);

  const char* rootpath = NULL;
  
  unsigned int getColumn();
  
  virtual ~CssWriter();
  virtual void writeAtRule(const Token &keyword, const TokenList &rule);
  virtual void writeRulesetStart(const TokenList &selector);
  virtual void writeRulesetEnd();
  virtual void writeDeclaration(const Token &property, const TokenList &value);
  virtual void writeDeclarationDeliminator();
  virtual void writeComment(const Token &comment);
  virtual void writeMediaQueryStart(const TokenList &selector);
  virtual void writeMediaQueryEnd();

  void writeSourceMapUrl(const char* sourcemap_url);
};
  
#endif
