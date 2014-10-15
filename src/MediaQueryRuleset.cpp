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

#include "MediaQueryRuleset.h"
#include "LessStylesheet.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

MediaQueryRuleset::MediaQueryRuleset(): LessRuleset() {
}
MediaQueryRuleset::~MediaQueryRuleset() {
}

void MediaQueryRuleset::process(Stylesheet* s, Selector* prefix) {
  MediaQuery* query = new MediaQuery();
  Ruleset* target;
  MediaQuery* parent;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing Less Ruleset: " <<
    *getSelector()->toString();
#endif
  
  query->setSelector(getSelector()->clone());

  parent = dynamic_cast<MediaQuery*>(s);
  
  if (parent != NULL) {
    delete query->getSelector()->shift();
    query->getSelector()->unshift(new Token("and", Token::IDENTIFIER));
    query->getSelector()->addPrefix(parent->getSelector());
    parent->getStylesheet()->addStatement(query);
  } else {
    s->addStatement(query);
  }

  if (prefix != NULL) {
    target = new Ruleset();
    target->setSelector(prefix->clone());

#ifdef WITH_LIBGLOG
    VLOG(3) << "Interpolating selector " <<
    *target->getSelector()->toString();
#endif
    getLessStylesheet()->getValueProcessor()->interpolateTokenList(target->getSelector());
  
    query->addStatement(target);
    insert(NULL, target);
  } else
    insert(NULL, query);
}


