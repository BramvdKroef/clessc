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
#include <getopt.h>

#include "LessTokenizer.h"
#include "LessParser.h"
#include "CssWriter.h"
#include "CssPrettyWriter.h"
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
  cout <<
    "Usage: lessc [OPTION]... [FILE]\n"
    "\n"
    "   FILE				Less source file. If not given, source \
is read from stdin.\n"
    "   -h, --help			Show this message and exit.\n"
    "       --version		Print the program name and version.\n"
    "\n"
    "   -o, --output=<FILE>		Send output to FILE\n"
    "   -f, --format			Format output CSS with newlines and \
indentation. By default the output is unformatted.\n"
    "\n"
    "   -m, --source-map=[FILE]	Generate a source map.\n"
    "       --source-map-rootpath=<PATH>   PATH is prepended to the \
source file references in the source map, and also to the source map \
reference in the css output. \n"
    "       --source-map-basepath=<PATH>   PATH is removed from the \
source file references in the source map, and also from the source \
map reference in the css output.\n"
    "\n"
    "   -v, --verbose=<LEVEL>	Output log data for debugging. LEVEL is \
a number in the range 1-3 that defines granularity.\n" 
    "\n"
    "Example:\n"
    "   lessc in.less -o out.css\n"
    "\n"
    "Report bugs to: " PACKAGE_BUGREPORT "\n"
    PACKAGE_NAME " home page: <" PACKAGE_URL ">\n";
}

void version () {
  cout <<
    PACKAGE_STRING "\n"
    "Copyright 2012 Bram van der Kroef\n"
    "License GPLv3+: GNU GPL version 3 or later \
<http://gnu.org/licenses/gpl.html>\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.\n";
}

bool parseInput(LessStylesheet &stylesheet,
                istream &in,
                const char* source,
                std::list<const char*> &sources){
  std::list<const char*>::iterator i;
  
  LessTokenizer tokenizer(in, source);
  LessParser parser(tokenizer, sources);
  
  try{
    parser.parseStylesheet(stylesheet);
  } catch(ParseException* e) {
#ifdef WITH_LIBGLOG
    LOG(ERROR) << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what();
#else
    cerr << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what();
#endif
    
    return false;
  } catch(exception* e) {
#ifdef WITH_LIBGLOG
    LOG(ERROR) << " Error: " << e->what();
#else
    cerr << " Error: " << e->what();
#endif
    return false;
  }
#ifdef WITH_LIBGLOG
  VLOG(1) << "Source files: ";
  for(i = sources.begin(); i != sources.end(); i++) {
    VLOG(1) << (*i);
  }
#endif
  return true;
}
void writeOutput (LessStylesheet &stylesheet,
                  CssWriter &writer) {
  Stylesheet css;
  ProcessingContext context;

  try{
    stylesheet.process(css, context);

  } catch(ParseException* e) {
#ifdef WITH_LIBGLOG
    LOG(ERROR) << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what();
#else
    cerr << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what();
#endif

    return;

  } catch(ValueException* e) {
#ifdef WITH_LIBGLOG
    LOG(ERROR) << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Error: " << e->what();
#else
    cerr << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Error: " << e->what();
#endif
    
    return;
  } catch(exception* e) {
#ifdef WITH_LIBGLOG
    LOG(ERROR) << "Error: " << e->what();
#else
    cerr << "Error: " << e->what();
#endif
    return;
  }

  css.write(writer);
}

int main(int argc, char * argv[]){
  istream* in = &cin;
  ostream* out = &cout;
  bool formatoutput = false;
  char* source = NULL;
  string output = "-";
  LessStylesheet stylesheet;
  std::list<const char*> sources;
  CssWriter* writer;

  std::string sourcemap_file = "";
  ostream* sourcemap_s = NULL;
  SourceMapWriter* sourcemap = NULL;
  const char* sourcemap_rootpath = NULL;
  const char* sourcemap_basepath = NULL;

  static struct option long_options[] = {
    {"version",    no_argument,       0, 1},
    {"help",       no_argument,       0, 'h'},
    {"output",     required_argument, 0, 'o'},
    {"format",     no_argument,       0, 'f'},
    {"verbose",    required_argument, 0, 'v'},
    {"source-map", optional_argument, 0, 'm'},
    {"source-map-rootpath", required_argument, 0, 2},
    {"source-map-basepath", required_argument, 0, 3},
    {0,0,0,0}
  };
  
#ifdef WITH_LIBGLOG
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  VLOG(1) << "Start.";
#endif
  
  try {
    int c, option_index;
#ifdef WITH_LIBGLOG
    VLOG(3) << "argc: " << argc;
#endif

    while((c = getopt_long(argc, argv, ":o:hfv:m::", long_options, &option_index)) != -1) {
      switch (c) {
      case 1:
        version();
        return 0;
      case 'h':
        usage();
        return 0;
      case 'o':
        output = optarg;
        out = new ofstream(optarg);
        break;
      case 'f':
        formatoutput = true;
        break;
      case 'v':
#ifdef WITH_LIBGLOG
        FLAGS_v = atoi(optarg);
#else
        std::cerr << "Warning: -v flag not supported: lessc has to be compiled with libglog.\n";
#endif
        break;
      case 'm':
        if (optarg)
          sourcemap_file = optarg;
        else
          sourcemap_file = "-";
        break;
        
      case 2:
        sourcemap_rootpath = optarg;
        break;
      case 3:
        sourcemap_basepath = optarg;
        break;
      }
    }
    
    if(argc - optind >= 1){
#ifdef WITH_LIBGLOG
      VLOG(1) << argv[optind];
#endif
      
      source = new char[std::strlen(argv[optind]) + 1];
      std::strcpy(source, argv[optind]);
      
      in = new ifstream(source);
      if (in->fail() || in->bad())
        throw new IOException("Error opening file");

    } else if (sourcemap_file == "-") {
      throw new IOException("source-map option requires that \
a file name is specified for either the source map or the less \
source.");
    } else {
      source = new char[2];
      std::strcpy(source, "-");
    }
    
    if (sourcemap_file == "-") {
      sourcemap_file = source;
      sourcemap_file += ".map";
    }

    sources.push_back(source);
    
    if (parseInput(stylesheet, *in, source, sources)) {
      if (sourcemap_file != "") {
#ifdef WITH_LIBGLOG
        VLOG(1) << "sourcemap: " << sourcemap_file;
#endif
        sourcemap_s = new ofstream(sourcemap_file.c_str());
        sourcemap = new SourceMapWriter(*sourcemap_s, sources, output.c_str(),
                                        sourcemap_rootpath,
                                        sourcemap_basepath);

        writer = formatoutput ? new CssPrettyWriter(*out, *sourcemap) :
          new CssWriter(*out, *sourcemap);
      } else {
        writer = formatoutput ? new CssPrettyWriter(*out) :
          new CssWriter(*out);
      }

      writeOutput(stylesheet, *writer);
      
      if (sourcemap != NULL) {
        if (sourcemap_basepath != NULL &&
            sourcemap_file.compare(0, std::strlen(sourcemap_basepath),
                                   sourcemap_basepath) == 0) {
          sourcemap_file.erase(0, std::strlen(sourcemap_basepath));
        }
        if (sourcemap_rootpath != NULL)
          sourcemap_file.insert(0, sourcemap_rootpath);
        
        writer->writeSourceMapUrl(sourcemap_file.c_str());
        sourcemap->close();
        delete sourcemap;
        if (sourcemap_s != NULL)
          delete sourcemap_s;
      }
      
      delete writer;
      *out << endl;
    } else
      return 1;
    delete source;
    
  } catch (IOException* e) {
#ifdef WITH_LIBGLOG
    LOG(ERROR) << " Error: " << e->what();
#else
    cerr << " Error: " << e->what();
#endif
    return 1;
  }
		
  return 0;
}

