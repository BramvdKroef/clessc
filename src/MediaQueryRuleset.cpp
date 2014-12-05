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

void MediaQueryRuleset::process(Stylesheet &s, Selector* prefix, ProcessingContext &context) {
  MediaQuery* query;
  Ruleset* target;
  Selector selector;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing Less Ruleset: " <<
    getSelector().toString();
#endif
  
  query = s.createMediaQuery();
  selector = getSelector();
  context.processValue(selector);

  if (query->getSelector().size() > 0) {
    selector.pop_front();

    query->getSelector().push_back(Token(" ", Token::WHITESPACE));
    query->getSelector().push_back(Token("and", Token::IDENTIFIER));
    query->getSelector().insert(query->getSelector().end(),
                                 selector.begin(),
                                 selector.end());
  } else
    query->setSelector(selector);

  if (prefix != NULL) {
    target = query->createRuleset();
    target->setSelector(*prefix);

#ifdef WITH_LIBGLOG
    VLOG(3) << "Interpolating selector " <<
    target->getSelector().toString();
#endif
    context.interpolate(target->getSelector());
  
    insert(NULL, *target, context);
  } else
    insert(NULL, *query, context);
}


