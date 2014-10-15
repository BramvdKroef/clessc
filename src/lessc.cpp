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
#include "CssWriter.h"
#include "CssPrettyWriter.h"
#include "ExtendCssWriter.h"
#include "Stylesheet.h"
#include "IOException.h"
#include "LessStylesheet.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

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


bool parseInput(LessStylesheet* stylesheet, istream* in, string source){
  LessTokenizer tokenizer(in, source);
  LessParser parser(&tokenizer);
  DLOG(INFO) << "Process input";
  
  try{
    parser.parseStylesheet(stylesheet);
  } catch(ParseException* e) {
    LOG(ERROR) << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what();

    return false;
  } catch(exception* e) {
    LOG(ERROR) << "Line " << tokenizer.getLineNumber() << ", Column " <<
      tokenizer.getColumn() << " Error: " << e->what();
    return false;
  }
  
  return true;
}
void writeOutput (ostream* out, LessStylesheet* stylesheet, bool format) {
  Stylesheet css;
  map<string,TokenList*> extensions;
  CssWriter *w1, *w2;
  w1 = format ? new CssPrettyWriter(out) : new CssWriter(out);

  try{
    stylesheet->process(&css);

    stylesheet->getExtensions(&extensions);
    
  } catch(ParseException* e) {
    LOG(ERROR) << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what();

    return;
  } catch(exception* e) {
    LOG(ERROR) << "Error: " << e->what();
    return;
  }

  w2 = new ExtendCssWriter(w1, &extensions);
  
  css.write(w2);
  *out << endl;
  delete w1;
  delete w2;
}

int main(int argc, char * argv[]){
  istream* in = &cin;
  ostream* out = &cout;
  bool formatoutput = false;
  string source = "-";
  LessStylesheet stylesheet;
  
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  DLOG(INFO) << "Start.";
  
  try {
    int c;
    DLOG(INFO) << "argc: " << argc;

    while((c = getopt(argc, argv, ":o:hfv:")) != EOF) {
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
      case 'v':
        FLAGS_v = atoi(optarg);
        break;
      }
    }
    
    if(argc - optind >= 1){
      DLOG(INFO) << argv[optind];
      in = new ifstream(argv[optind]);
      source = argv[optind];
      if (in->fail() || in->bad())
        throw new IOException("Error opening file");
    }
    
    if (parseInput(&stylesheet, in, source)) {
        writeOutput(out, &stylesheet, formatoutput);
    } else
      return 1;

  } catch (IOException* e) {
    LOG(ERROR) << " Error: " << e->what();
    return 1;
  }
		
  return 0;
}

