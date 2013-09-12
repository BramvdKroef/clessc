/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>

#include "LessTokenizer.h"
#include "LessParser.h"
#include "value/ValueProcessor.h"
#include "ParameterRulesetLibrary.h"
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
  ValueProcessor vp;
  ParameterRulesetLibrary pr(&vp);
  LessTokenizer tokenizer(in);
  LessParser parser(&tokenizer, &pr, &vp);
  Stylesheet* s = new Stylesheet();
  
  try{
    parser.parseStylesheet(s);
  } catch(exception* e) {
    cerr << "Line " << tokenizer.getLineNumber() << ", Column " << 
      tokenizer.getColumn() << " Parse Error: " << e->what() << endl;
    return NULL;
  }
  
  return s;
}
void writeOutput (ostream* out, Stylesheet* stylesheet, bool format) {
  CssWriter *w;
  w = format ? new CssPrettyWriter(out) : new CssWriter(out);
  
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

