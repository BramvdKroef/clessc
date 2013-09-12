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

#include <config.h>

#ifdef WITH_LIBPNG
#include <png.h>
#endif

#ifdef WITH_LIBJPEG
#include <jpeglib.h>
#include <setjmp.h>

struct urlvalue_jpeg_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct urlvalue_jpeg_error_mgr * urlvalue_jpeg_error_ptr;

METHODDEF(void)
urlvalue_jpeg_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  urlvalue_jpeg_error_ptr myerr = (urlvalue_jpeg_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

#endif

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
  loaded = loadPng() || loadJpeg();
   
  return loaded;
}

bool UrlValue::loadPng() {
  if (loaded != false)
    return true;

#ifdef WITH_LIBPNG
  unsigned char header[8];    // 8 is the maximum size that can be checked
  /* open file and test for it being a png */
  
  png_structp png_ptr;
  png_infop info_ptr;

  FILE *fp = fopen(path.c_str(), "rb");
  if (!fp)
    return false; //"Image file could not be opened"

  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8))
    return false; //"Image is not a PNG file"
  
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

  width = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);

  fclose(fp);
  return true;
  
#else
  return false;
#endif  
}


bool UrlValue::loadJpeg() {
  if (loaded != false)
    return true;
  
#ifdef WITH_LIBJPEG
  struct jpeg_decompress_struct cinfo;
  
  struct urlvalue_jpeg_error_mgr jerr;
  /* More stuff */
  FILE * infile;
  JSAMPARRAY buffer;	/* Output row buffer */
  int row_stride;	/* physical row width in output buffer */
  
  if ((infile = fopen(path.c_str(), "rb")) == NULL) {
    return false;
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = urlvalue_jpeg_error_exit;
  /* Establish the setjmp return context for urlvalue_jpeg_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return false;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   * (a) suspension is not possible with the stdio data source, and
   * (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.txt for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  width = cinfo.output_width;
  height = cinfo.output_height;
  
  /* We may need to do some setup of our own at this point before reading
   * the data. After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /* jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    //put_scanline_someplace(buffer[0], row_stride);
  }

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above. (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);
  return true;
#else
  return false;
#endif
}

unsigned int UrlValue::getImageWidth() {
  return (loadImg() ? width : 0);
}
unsigned int UrlValue::getImageHeight() {
  return (loadImg() ? height : 0);
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
