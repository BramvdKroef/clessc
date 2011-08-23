#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Tokenizer.h"
#include "Parser.h"
#include "IOException.h"

using namespace std;

/**
 * /main Interpreter
 * 
 */

void usage () {
  cout << "  cat infile.txt | Interpreter > output.txt" << endl;
}

/*	
	pre: msg wijst naar een array can len chars
	post: de tekst in msg wordt naar het bestand en het schem geschreven 
*/
void out(const char* msg, int len){
  cout.write(msg, len);
}



void processInput(istream* in){
  Tokenizer* tokenizer = new Tokenizer(in);
  Parser* parser = new Parser(tokenizer);
  
  string str;
	
  try{
    bool b = parser->parseStylesheet();
    
  } catch(ParseException* e) {
    ostringstream stream1(ostringstream::out);
    stream1 << "Line " << tokenizer->getLineNumber() << ", Collumn " << 
      tokenizer->getPosition() << " Parse Error: " << e->what() << endl; 
    cout << stream1.str();
  }
  
  delete tokenizer;
  delete parser;
}

int main(int argc, char * argv[]){
  string helpstr("-h");
  if (argc > 1 && helpstr.compare(argv[1]) == 0) {
    usage();
    return 0;
  }

  try {
    if(argc <= 1){
      cout << "For help run 'Interpreter -h'" << endl;
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
    string str;
    stream1 << " Error: " << e->what() << endl; 
    str = stream1.str();
    out(str.c_str(), str.size());
  }
		
  return 0;
}

