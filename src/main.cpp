#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>

#include "LessTokenizer.h"
#include "LessParser.h"
#include "CssWriter.h"
#include "CssPrettyWriter.h"
#include "Stylesheet.h"
#include "IOException.h"

using namespace std;

/**
 * /main Less CSS compiler, implemented in C++.
 * 
 */

void usage () {
  cout << "Usage: lessc [OPTION] FILE" << endl;
  cout << endl;
  cout << "   -o FILE	Send output to FILE" << endl;
  cout << "   -f    	Format output." << endl;
  cout << endl;
  cout << "Example:" << endl;
  cout << "   lessc in.less -o out.css" << endl;
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
void writeOutput (ostream* out, Stylesheet* stylesheet, bool format) {
  CssWriter *w;
  if (format)
    w = new CssPrettyWriter(out);
  else
    w = new CssWriter(out);
  
  w->writeStylesheet(stylesheet);
  *out << endl;
  delete w;
}

int main(int argc, char * argv[]){
  Stylesheet* s;
  istream* in = &cin;
  ostream* out = &cout;
  bool formatoutput = false;
  
  try {
    int c;
    while((c = getopt(argc, argv, ":o:hf")) != EOF) {
      switch (c) {
      case 'h':
        usage();
        return 0;
      case 'o':
        out = new ofstream(optarg);
        break;
      case 'f':
        formatoutput = true;
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
      writeOutput(out, s, formatoutput);
      delete s;
    } else
      return 1;

  } catch (IOException* e) {
    cerr << " Error: " << e->what() << endl;
    return 1;
  }
		
  return 0;
}

