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
  cout << "  cat infile.txt | clessc > output.txt" << endl;
}


void processInput(istream* in){
  LessTokenizer* tokenizer = new LessTokenizer(in);
  LessParser* parser = new LessParser(tokenizer);
  CssWriter *w = new CssWriter(&cout);
  
  try{
    Stylesheet* s = parser->parseStylesheet();
    w->writeStylesheet(s);
    cout << endl;
    delete s;
  } catch(ParseException* e) {
    cerr << "Line " << tokenizer->getLineNumber() << ", Collumn " << 
      tokenizer->getPosition() << " Parse Error: " << e->what() << endl; 
  }
  
  delete tokenizer;
  delete parser;
  delete w;
}

int main(int argc, char * argv[]){
  string helpstr("-h");
  if (argc > 1 && helpstr.compare(argv[1]) == 0) {
    usage();
    return 0;
  }

  try {
    if(argc <= 1){
      processInput(&cin);
    } else {
      for (int i=1; i < argc; i++){
        ifstream* file = new ifstream(argv[i]);
        
        if (file->fail() || file->bad())
          throw new IOException("Error opening file");

        processInput(file);
        file->close();
        delete file;
      }
    }
  } catch (IOException* e) {
    ostringstream stream1(ostringstream::out);
    cerr << " Error: " << e->what() << endl; 
  }
		
  return 0;
}

