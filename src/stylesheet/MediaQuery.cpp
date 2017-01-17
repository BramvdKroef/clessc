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

#include "MediaQuery.h"

Selector& MediaQuery::getSelector() {
  return selector;
}
void MediaQuery::setSelector(const Selector &s) {
  selector = s;
}

MediaQuery* MediaQuery::createMediaQuery() {
  MediaQuery* q = getStylesheet()->createMediaQuery();

#ifdef WITH_LIBGLOG
  VLOG(3) << "Creating media query";
#endif

  q->setSelector(getSelector());
  
  return q;
}

void MediaQuery::process(Stylesheet &s) {
  MediaQuery* query = s.createMediaQuery();
    
#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing media query " << getSelector().toString();
#endif
      
  query->setSelector(getSelector());
    
  Stylesheet::process(*query);
}

void MediaQuery::write(CssWriter &writer) {
  writer.writeMediaQueryStart(selector);
  Stylesheet::write(writer);
  
  writer.writeMediaQueryEnd();
}
