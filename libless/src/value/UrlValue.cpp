#include "less/value/UrlValue.h"
#include "less/value/FunctionLibrary.h"

#ifdef WITH_LIBPNG
#include <png.h>
#endif

#ifdef WITH_LIBJPEG
#include <jpeglib.h>
#include <setjmp.h>
#include <stdio.h>

struct urlvalue_jpeg_error_mgr {
  struct jpeg_error_mgr pub; /* "public" fields */

  jmp_buf setjmp_buffer; /* for return to caller */
};

typedef struct urlvalue_jpeg_error_mgr* urlvalue_jpeg_error_ptr;

METHODDEF(void)
urlvalue_jpeg_error_exit(j_common_ptr cinfo) {
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  urlvalue_jpeg_error_ptr myerr = (urlvalue_jpeg_error_ptr)cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message)(cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

#endif

UrlValue_Img::UrlValue_Img() {
}

UrlValue::UrlValue(Token& token, std::string& path) : Value() {
  tokens.push_back(token);
  this->path = path;
  type = Value::URL;
}

UrlValue::~UrlValue() {
}

std::string UrlValue::getPath() const {
  return path;
}

std::string UrlValue::getRelativePath() const {
  std::string source = tokens.front().source;
  size_t pos = source.find_last_of("/\\");
  std::string relative_path;

  // if the source stylesheet is not in the current working directory
  //  then add its directory to the path.
  if (pos != std::string::npos) {
    relative_path.append(source.substr(0, pos + 1));
    relative_path.append(this->path);
  } else
    relative_path = this->path;
  return relative_path;
}

Value* UrlValue::operator+(const Value& v) const {
  (void)v;
  throw new ValueException("You can not add urls.", *this->getTokens());
}
Value* UrlValue::operator-(const Value& v) const {
  (void)v;
  throw new ValueException("You can not substract urls.", *this->getTokens());
}
Value* UrlValue::operator*(const Value& v) const {
  (void)v;
  throw new ValueException("You can not multiply urls.", *this->getTokens());
}
Value* UrlValue::operator/(const Value& v) const {
  (void)v;
  throw new ValueException("You can not divide urls.", *this->getTokens());
}

bool UrlValue::operator<(const Value& v) const {
  const UrlValue* u;
  const BooleanValue* b;

  if (v.type == URL) {
    u = static_cast<const UrlValue*>(&v);
    return (path < u->getPath());

  } else if (v.type == BOOLEAN) {
    b = static_cast<const BooleanValue*>(&v);
    return b->getValue();

  } else {
    throw new ValueException("You can only compare urls with urls.",
                             *this->getTokens());
  }
}
bool UrlValue::operator==(const Value& v) const {
  const UrlValue* u;
  const BooleanValue* b;

  if (v.type == URL) {
    u = static_cast<const UrlValue*>(&v);
    return (path == u->getPath());

  } else if (v.type == BOOLEAN) {
    // any url is falsy.
    b = static_cast<const BooleanValue*>(&v);
    return (false == b->getValue());
  
  } else {
    throw new ValueException("You can only compare urls with urls.",
                             *this->getTokens());
  }
}

bool UrlValue::loadImg(UrlValue_Img& img) const {
  return loadPng(img) || loadJpeg(img);
}

bool UrlValue::loadPng(UrlValue_Img& img) const {
#ifdef WITH_LIBPNG
  unsigned char header[8];  // 8 is the maximum size that can be checked
  /* open file and test for it being a png */

  png_structp png_ptr;
  png_infop info_ptr;
  png_byte color_type;
  int channels;

  std::string path = getRelativePath();

  FILE* fp = fopen(path.c_str(), "rb");
  if (!fp)
    return false;  //"Image file could not be opened"

  fread(header, 1, 8, fp);

  if (png_sig_cmp(header, 0, 8))
    return false;  //"Image is not a PNG file"

  /* initialize stuff */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr)
    throw new ValueException("png_create_read_struct failed",
                             *this->getTokens());

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    throw new ValueException("png_create_info_struct failed",
                             *this->getTokens());

  if (setjmp(png_jmpbuf(png_ptr)))
    throw new ValueException("Error during init_io", *this->getTokens());

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  img.width = png_get_image_width(png_ptr, info_ptr);
  img.height = png_get_image_height(png_ptr, info_ptr);
  channels = png_get_channels(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);
    channels = 3;
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
    channels += 1;
  }

  png_color_16p pBackground;
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD)) {
    png_get_bKGD(png_ptr, info_ptr, &pBackground);
    img.background.setRGB(
        pBackground->red, pBackground->green, pBackground->blue);
  } else {
    img.background.setRGB(255, 255, 255);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  png_ptr = NULL;
  info_ptr = NULL;
  fclose(fp);

  return true;

#else
  return false;
#endif
}

bool UrlValue::loadJpeg(UrlValue_Img& img) const {
#ifdef WITH_LIBJPEG
  struct jpeg_decompress_struct cinfo;

  struct urlvalue_jpeg_error_mgr jerr;
  /* More stuff */
  FILE* infile;
  JSAMPARRAY buffer; /* Output row buffer */
  int row_stride;    /* physical row width in output buffer */
  std::string path = getRelativePath();
  unsigned int rgb[3];

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

  (void)jpeg_read_header(&cinfo, TRUE);
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

  (void)jpeg_start_decompress(&cinfo);
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
  buffer = (*cinfo.mem->alloc_sarray)(
      (j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

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
    (void)jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    // put_scanline_someplace(buffer[0], row_stride);

    if (cinfo.out_color_space == JCS_RGB &&
        (cinfo.output_scanline == 1 ||
         cinfo.output_scanline == cinfo.output_height)) {
      if (cinfo.output_scanline == 1) {
        rgb[0] = buffer[0][0];
        rgb[1] = buffer[0][1];
        rgb[2] = buffer[0][2];

        if (rgb[0] != buffer[0][row_stride - cinfo.output_components] ||
            rgb[1] != buffer[0][row_stride - cinfo.output_components + 1] ||
            rgb[2] != buffer[0][row_stride - cinfo.output_components + 2]) {
          rgb[0] = rgb[1] = rgb[2] = 0;
        }
      } else if (rgb[0] != buffer[0][0] ||
                 rgb[1] != buffer[0][1] ||
                 rgb[2] != buffer[0][2] ||
                 rgb[0] != buffer[0][row_stride - cinfo.output_components] ||
                 rgb[1] != buffer[0][row_stride - cinfo.output_components + 1] ||
                 rgb[2] != buffer[0][row_stride - cinfo.output_components + 2]) {
        rgb[0] = rgb[1] = rgb[2] = 0;
      }
    }
  }
  img.background.setRGB(rgb[0], rgb[1], rgb[2]);
  
  /* Step 7: Finish decompression */

  (void)jpeg_finish_decompress(&cinfo);
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

