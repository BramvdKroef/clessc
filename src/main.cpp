#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "LessTokenizer.h"
#include "LessParser.h"
#include "CssWriter.h"
#include "Stylesheet.h"
#include "IOException.h"

using namespace std;

/**
 * /main Less CSS compiler, implemented in C++.
 * 
 */

void usage () {
  cout << "  clessc in.less -o out.css" << endl;
}


Stylesheet* processInput(istream* in){
  LessTokenizer* tokenizer = new LessTokenizer(in);
  LessParser* parser = new LessParser(tokenizer);
  Stylesheet* s;
  
  try{
    s = new Stylesheet();
    parser->parseStylesheet(s);
  } catch(exception* e) {
    cerr << "Line " << tokenizer->getLineNumber() << ", Collumn " << 
      tokenizer->getColumn() << " Parse Error: " << e->what() << endl;
    return NULL;
  }
  
  delete tokenizer;
  delete parser;
  return s;
}
void writeOutput (ostream* out, Stylesheet* stylesheet) {
  CssWriter *w = new CssWriter(out);
  w->writeStylesheet(stylesheet);
  *out << endl;
  delete w;
}

int main(int argc, char * argv[]){
  Stylesheet* s;
  istream* in = &cin;
  ostream* out = &cout;
  
  try {
    int c;
    while((c = getopt(argc, argv, ":o:h")) != EOF) {
      switch (c) {
      case 'h':
        usage();
        return 0;
      case 'o':
        out = new ofstream(optarg);
        break;
      }
    }

    if(argc - optind >= 1){
      in = new ifstream(argv[optind]);
      if (in->fail() || in->bad())
        throw new IOException("Error opening file");
    }

    s = processInput(in);
    if (s != NULL) {
      writeOutput(out, s);
      delete s;
    }

  } catch (IOException* e) {
    cerr << " Error: " << e->what() << endl; 
  }
		
  return 0;
}

