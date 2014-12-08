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

#include "Extension.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

Extension::Extension() {
  all = false;
}

Extension::Extension(bool all) {
  all = true;
}
Extension::~Extension() {
}

Selector& Extension::getTarget() {
  return target;
}
Selector& Extension::getExtension() {
  return extension;
}

void Extension::setExtension(Selector &selector) {
  extension = selector;
}

void Extension::updateSelector(Selector &s) const {
  if (target.back() == "all") {
    replaceInSelector(s);
  } else if (s.match(target)) {

#ifdef WITH_LIBGLOG
    VLOG(2) << "Extending " << s.toString() << " with " << extension.toString() ;
#endif

    // add comma and selector    
    s.push_back(Token(",",Token::OTHER));
    s.insert(s.end(), extension.begin(), extension.end());
  }
}
void Extension::replaceInSelector(Selector &s) const {
  Selector t = target;
  Selector::const_iterator end, pos, first, last;
  t.pop_back();
  t.rtrim();

  end = s.end();

  for (first = s.begin(); first != end;) {
    last = s.findComma(first, end);
    pos = s.find(t, first, last);

    if (pos != last) {
#ifdef WITH_LIBGLOG
      VLOG(2) << "Extending " << s.toString() << " with " << extension.toString() ;
#endif
      
      s.push_back(Token(",", Token::OTHER));

      // fix position of <code>last</code>. If <code>last ==
      // end</code> it moves up as tokens are inserted, and we want it
      // to stay where it is.
      if (last == end)
        last--;
      
      s.insert(s.end(), first, pos);
      s.insert(s.end(), extension.begin(), extension.end());
      pos = s.walk(t, pos);
      s.insert(s.end(), pos, last);
    }
    
    first = last;
    if (first != end)
      first++;
  }
}
