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
#include <stdio.h>

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

UrlValue::UrlValue(Token &token, std::string &path): Value() {
  tokens.push_back(token);
  this->path = path;
  type = Value::URL;
}


UrlValue::~UrlValue() {
}

std::string UrlValue::getPath() const {
  return path;
}

Value* UrlValue::add(const Value &v) const {
  (void)v;
  throw new ValueException("You can not add urls.");
}
Value* UrlValue::substract(const Value &v) const {
  (void)v;
  throw new ValueException("You can not substract urls.");
}
Value* UrlValue::multiply(const Value &v) const {
  (void)v;
  throw new ValueException("You can not multiply urls.");
}
Value* UrlValue::divide(const Value &v) const {
  (void)v;
  throw new ValueException("You can not divide urls.");
}

BooleanValue* UrlValue::lessThan(const Value &v) const {
  const UrlValue* u;
  if (v.type == URL) {
    u = static_cast<const UrlValue*>(&v);
    return new BooleanValue(path < u->getPath());
  } else {
    throw new ValueException("You can only compare urls with urls.");
  }
}
BooleanValue* UrlValue::equals(const Value &v) const {
  const UrlValue* u;
  
  if (v.type == URL) {
    u = static_cast<const UrlValue*>(&v);
    return new BooleanValue(path == u->getPath());
  } else {
    throw new ValueException("You can only compare urls with urls.");
  }
}

bool UrlValue::loadImg(UrlValue_Img &img) const {
  return loadPng(img) || loadJpeg(img);
}

bool UrlValue::loadPng(UrlValue_Img &img) const {

#ifdef WITH_LIBPNG
  unsigned char header[8];    // 8 is the maximum size that can be checked
  /* open file and test for it being a png */
  
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep* row_pointers;
  png_uint_32 rowbytes;
  unsigned int y;
    
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

  img.width = png_get_image_width(png_ptr, info_ptr);
  img.height = png_get_image_height(png_ptr, info_ptr);
  int channels = png_get_channels(png_ptr, info_ptr);
  
  if(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);
    channels = 3;
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
    channels += 1;
  }

  /* read file */
  if (setjmp(png_jmpbuf(png_ptr)))
    throw new ValueException("Error during read_image");

  rowbytes = png_get_rowbytes(png_ptr,info_ptr);
  
  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * img.height);

  for (y = 0; y < img.height; y++)
    row_pointers[y] = (png_byte*)malloc(rowbytes);

  png_read_image(png_ptr, row_pointers);

  if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB ||
      png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGBA ||
      png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE) {

    png_byte* color1 = row_pointers[0];
    png_byte* color2 = row_pointers[0] + (img.width - 1) * channels;
    png_byte* color3 = row_pointers[img.height - 1];
    png_byte* color4 = row_pointers[img.height - 1] + (img.width - 1) * channels;

    if (memcmp(color1, color2, channels) == 0 &&
        memcmp(color1, color3, channels) == 0 &&
        memcmp(color1, color4, channels) == 0) {
        
      img.background.setRGB(color1[0], color1[1], color1[2]);
      if (channels == 4)
        img.background.setAlpha(color1[3]);
    } else {
      img.background.setRGB(0,0,0);
    }
  }
    
  fclose(fp);
  return true;
  
#else
  return false;
#endif  
}


bool UrlValue::loadJpeg(UrlValue_Img &img) const {
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

  img.width = cinfo.output_width;
  img.height = cinfo.output_height;
  
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
    if (cinfo.out_color_space == JCS_RGB && 
        (cinfo.output_scanline == 1 ||
         cinfo.output_scanline == cinfo.output_height)) {

      if (cinfo.output_scanline == 1) {
        img.background.setRGB(buffer[0][0], buffer[0][1], buffer[0][2]);
      }
      if (img.background.getRed() != buffer[0][0] ||
          img.background.getGreen() != buffer[0][1] ||
          img.background.getBlue() != buffer[0][2] ||

          img.background.getRed() != buffer[0][row_stride - cinfo.output_components] ||
          img.background.getGreen() != buffer[0][row_stride - cinfo.output_components + 1] ||
          img.background.getBlue() != buffer[0][row_stride - cinfo.output_components + 2]) {
        img.background.setRGB(0,0,0);
      }
    }
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

unsigned int UrlValue::getImageWidth() const {
  UrlValue_Img img;
  return (loadImg(img) ? img.width : 0);
}
unsigned int UrlValue::getImageHeight() const {
  UrlValue_Img img;
  return (loadImg(img) ? img.height : 0);
}

Color UrlValue::getImageBackground() const {
  UrlValue_Img img;
  loadImg(img);
  return img.background;
}


void UrlValue::loadFunctions(FunctionLibrary &lib) {
  lib.push("imgheight", "R", &UrlValue::imgheight);
  lib.push("imgwidth", "R", &UrlValue::imgwidth);
  lib.push("imgbackground", "R", &UrlValue::imgbackground);
}


Value* UrlValue::imgheight(const vector<const Value*> &arguments) {
  const UrlValue* u;
  NumberValue* val;
  std::string px = "px";

  u = static_cast<const UrlValue*>(arguments[0]);
  val = new NumberValue(u->getImageHeight(), Token::DIMENSION, &px);
  return val;
}
Value* UrlValue::imgwidth(const vector<const Value*> &arguments) {
  const UrlValue* u;
  NumberValue* val;
  std::string px = "px";

  u = static_cast<const UrlValue*>(arguments[0]);
  val = new NumberValue(u->getImageWidth(), Token::DIMENSION, &px);
  return val;
}
 
Value* UrlValue::imgbackground(const vector<const Value*> &arguments) {
  const UrlValue* u = static_cast<const UrlValue*>(arguments[0]);
  return new Color(u->getImageBackground());
}
