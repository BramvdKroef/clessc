#ifndef __Parser_h__
#define __Parser_h__

#include "Tokenizer.h"


/*   Parser
   De Parser leest Symbols uit een Symbolizer en structureerd ze in een boomstructuur zoals
   die wordt ondersteund door Expression. 
   parseExpression() leest een hele expressie uit. Elke expressie wordt afgesloten met een 
   EOL of EOS symbool. Elk Symbol van het type BINARY(binaire operator) krijgt twee kinderen
   (vertakkingen), UNARY(unaire operatoren) en PREFIX('-' of '+') krijgen 1 kind. Variabelen
   en Constanten krijgen 0 kinderen toegewezen.
   De SymbolTable wordt geraadpleegt om uit te zoeken welke BINAIRE operator het meest 
   significant is.
   Een ParseException wordt gegeven als:
     - Voor een operator niet het juiste aantal kinderen uit de Symbolizer gelezen kan 
       worden.
     - Als twee pimaire operatoren elkaar volgen.
     - Als een unaire operator gegeven wordt zonder voorgaande primaire expressie.
     - Bij een subexpressie (beginnende met een '(') niet is afgesloten met een ')'.
*/

class Parser{
 private:
  Tokenizer* tokenizer;
  void skipWhitespace();
  bool parseStatement();
  bool parseAtRule ();
  bool parseBlock ();
  bool parseRuleset ();
  bool parseSelector();
  bool parseDeclaration ();
  bool parseProperty ();
  bool parseValue ();
  bool parseAny ();
  bool parseUnused();
  
 public:
  Parser(Tokenizer* tokenizer);
  bool parseStylesheet();
  
  
};

#endif
