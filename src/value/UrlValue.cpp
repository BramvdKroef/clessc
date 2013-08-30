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

#include "UrlValue.h"

UrlValue::UrlValue(Token* token, string path): Value() {
  tokens.push(token);
  this->path = path;
  type = Value::URL;
  loaded = false;
}


UrlValue::~UrlValue() {
}

Value* UrlValue::add(Value* v) {
  (void)v;
  throw new ValueException("You can not add images.");
}
Value* UrlValue::substract(Value* v) {
  (void)v;
  throw new ValueException("You can not substract images.");
}
Value* UrlValue::multiply(Value* v) {
  (void)v;
  throw new ValueException("You can not multiply images.");
}
Value* UrlValue::divide(Value* v) {
  (void)v;
  throw new ValueException("You can not divide images.");
}
int UrlValue::compare(Value* v) {
  if (v->type == URL) {
    return 0;
  } else {
    throw new ValueException("You can only compare images with images.");
  }
}

bool UrlValue::loadImg() {
  if (loaded != false)
    return true;
  
  unsigned char header[8];    // 8 is the maximum size that can be checked
  /* open file and test for it being a png */
  FILE *fp = fopen(path.c_str(), "rb");
  
  if (!fp)
    throw new ValueException("Image file could not be opened");
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8))
    throw new ValueException("Image is not a PNG file");
  
  /* initialize stuff */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr)
    throw new ValueException("png_create_read_struct failed");

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    throw new ValueException("png_create_info_struct failed");

  if (setjmp(png_jmpbuf(png_ptr)))
    throw new ValueException("Error during init_io");

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);
  fclose(fp);

  loaded = true;
  return true;
}

unsigned int UrlValue::getImageWidth() {
  loadImg();
  return png_get_image_width(png_ptr, info_ptr);
}
unsigned int UrlValue::getImageHeight() {
  loadImg();
  return png_get_image_height(png_ptr, info_ptr);
}

void UrlValue::loadFunctions(FunctionLibrary* lib) {
  lib->push("imgheight", "R", &UrlValue::imgheight);
  lib->push("imgwidth", "R", &UrlValue::imgwidth);
}


Value* UrlValue::imgheight(vector<Value*> arguments) {
  NumberValue* val = new NumberValue(new Token("1", Token::NUMBER));
  val->setValue(((UrlValue*)arguments[0])->getImageHeight());
  val->setUnit("px");
  return val;
}
Value* UrlValue::imgwidth(vector<Value*> arguments) {
  NumberValue* val = new NumberValue(new Token("1", Token::NUMBER));
  
  val->setUnit("px");
  val->setValue((double)((UrlValue*)arguments[0])->getImageWidth());
  return val;
}
