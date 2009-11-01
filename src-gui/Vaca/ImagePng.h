// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the author nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef VACA_IMAGE_PNG_H
#define VACA_IMAGE_PNG_H

#include <memory>

#include "Vaca/base.h"
#include "Vaca/Image.h"
#include "Vaca/Exception.h"
#include "Vaca/Image.h"

#include "Vaca/Graphics.h"
#include "Vaca/Color.h"

#define PNG_NO_TYPECAST_NULL
#include "png.h"

namespace Vaca {

  class LoadImageException : public Exception
  {
  public:

    LoadImageException() : Exception() { }
    LoadImageException(const String& message) : Exception(message) { }
    virtual ~LoadImageException() throw() { }

  };

  class ImagePng
  {

    class FileWrapper
    {
      FILE* m_fp;

    public:

      FileWrapper(const String& fileName) {
	m_fp = fopen(convert_to<std::string>(fileName).c_str(), "rb");
	if (!m_fp)
	  throw LoadImageException(L"Error loading file");
      }

      ~FileWrapper() {
	fclose(m_fp);
      }

      operator FILE*() {
	return m_fp;
      }

    };				// FileWrapper

    class PngInfoWrapper
    {
      png_structp m_png_ptr;
      png_infop m_info_ptr;

    public:

      PngInfoWrapper(png_structp png_ptr)
	: m_png_ptr(png_ptr)
      {
	// Allocate/initialize the memory for image information.
	m_info_ptr = png_create_info_struct(m_png_ptr);
	if (m_info_ptr == NULL)
	  throw LoadImageException(L"Cannot create PNG internal structure (error 2)\n");
      }

      ~PngInfoWrapper()
      {
	png_destroy_info_struct(m_png_ptr, &m_info_ptr);
      }

      operator png_infop()
      {
	return m_info_ptr;
      }

      png_infop operator->()
      {
	return m_info_ptr;
      }

    };

    class PngWrapper
    {

      static void report_png_error(png_structp png_ptr, png_const_charp error)
      {
	ImagePng* image_png = (ImagePng*)png_ptr->error_ptr;
	image_png->reportError(convert_to<String>(error));
      }

      png_structp m_png_ptr;

    public:

      PngWrapper(png_voidp data) {
	m_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, data,
					   report_png_error, report_png_error);
	if (!m_png_ptr)
	  throw LoadImageException(L"Cannot create PNG internal structures (error 1)");
      }

      ~PngWrapper() {
	png_destroy_read_struct(&m_png_ptr, png_infopp_NULL, png_infopp_NULL);
      }

      Image load(FILE* fp)
      {
	ImagePng* image_png = (ImagePng*)m_png_ptr->error_ptr;

	// Set error handling if you are using the setjmp/longjmp method (this is
	// normal method of doing things with libpng).
	if (setjmp(png_jmpbuf(m_png_ptr))) {
	  // If we get here, we had a problem reading the file
	  throw LoadImageException(L"Error reading PNG file");
	}

	PngInfoWrapper png_info(m_png_ptr);

	// int bit_depth, color_type, interlace_type;
	// png_colorp palette;
	// int imgtype;

	// Set up the input control if you are using standard C streams
	png_init_io(m_png_ptr, fp);

	/* If we have already read some of the signature */
	unsigned int sig_read = 0;
	png_set_sig_bytes(m_png_ptr, sig_read);

	/* The call to png_read_info() gives us all of the information from the
	 * PNG file before the first IDAT (image data chunk).
	 */
	png_read_info(m_png_ptr, png_info);
  
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	png_get_IHDR(m_png_ptr, png_info, &width, &height, &bit_depth, &color_type,
		     &interlace_type, int_p_NULL, int_p_NULL);

	// 16bits to 8bits
	png_set_strip_16(m_png_ptr);

	// We get RGB always
	png_set_gray_to_rgb(m_png_ptr);
	png_set_palette_to_rgb(m_png_ptr);

	int number_passes = png_set_interlace_handling(m_png_ptr);

	// Update PNG info with the all conversions
	png_read_update_info(m_png_ptr, png_info);

	// Create the image
	Image image(width, height);
	ImagePixels pixels = image.getPixels();

	png_bytep row_pointer =
	  (png_bytep)png_malloc(m_png_ptr, png_get_rowbytes(m_png_ptr, png_info));
	png_uint_32 x, y;

	for (int pass = 0; pass < number_passes; pass++) {
	  for (y = 0; y < height; y++) {
	    png_read_row(m_png_ptr, row_pointer, png_bytepp_NULL);

	    switch (png_info->color_type) {
	      case PNG_COLOR_TYPE_RGB_ALPHA: {
		register BYTE *src_address = row_pointer;
		register int x, r, g, b, a;

		for (x=0; x<width; x++) {
		  r = *(src_address++);
		  g = *(src_address++);
		  b = *(src_address++);
		  a = *(src_address++);
		  pixels.setPixel(x, y, ImagePixels::makePixel(r, g, b, a));
		}
		break;
	      }
	      case PNG_COLOR_TYPE_RGB: {
		register BYTE *src_address = row_pointer;
		register int x, r, g, b;

		for (x=0; x<width; x++) {
		  r = *(src_address++);
		  g = *(src_address++);
		  b = *(src_address++);
		  pixels.setPixel(x, y, ImagePixels::makePixel(r, g, b, 255));
		}
		break;
	      }
	    }

	    image_png->reportProgress((float)((float)pass + (float)(y+1) / (float)(height))
				      / (float)number_passes);
	    if (image_png->isStop())
	      break;
	  }
	}
	png_free(m_png_ptr, row_pointer);

	image.setPixels(pixels);

	return image;
      }

    };				// PngWrapper

    String m_errors;

  public:

    static Image load(const String& fileName)
    {
      std::auto_ptr<ImagePng> ptr(new ImagePng());
      return ptr.get()->loadImage(fileName);
    }

    ImagePng()
    {
    }

    virtual void reportProgress(double progress)
    {
      // do nothing
    }

    virtual void reportError(const String& error)
    {
      m_errors += error;
    }

    virtual bool isStop()
    {
      return false;
    }

    Image loadImage(const String& fileName)
    {
      m_errors.clear();

      FileWrapper file(fileName);
      PngWrapper png_read((png_voidp)this);

      return png_read.load(file);
    }

  };				// class ImagePng

} // namespace Vaca

#endif // VACA_IMAGE_H
