/**************************************************************************
 *
 * \file
 *
 * \brief IJG JPEG library data input module.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 ***************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "compiler.h"
#include "board.h"

#include "et024006dhu.h"
#include "jpeg_decoder.h"
#include "conf_jpeg_decoder.h"

// IJG jpeg decoder
#include "jpeglib.h"
#include "jinclude.h"

struct jpeg_lib
{
	uint8_t *output_image;
	struct jpeg_decompress_struct *cinfo;

};
static struct jpeg_lib jpeg_lib_data;

/* If the image is bigger than these values it will be resized
 * by a factor of 1/2, 1/3, 1/4 .... until it fits */

#define DEFAULT_MAX_IMAGE_WIDTH     128
#define DEFAULT_MAX_IMAGE_HEIGHT    128
#define DEFAULT_PIXEL_SIZE_IN_BYTES 2
#define JPEG_INPUT_BUF_SIZE         1024

#if !defined(JPEG_DECODER_MAX_IMAGE_WIDTH)
  #define JPEG_DECODER_MAX_IMAGE_WIDTH      DEFAULT_MAX_IMAGE_WIDTH
#endif
#if !defined(JPEG_DECODER_MAX_IMAGE_HEIGHT)
  #define JPEG_DECODER_MAX_IMAGE_HEIGHT     DEFAULT_MAX_IMAGE_HEIGHT
#endif
#if !defined(JPEG_DECODER_PIXEL_SIZE_IN_BYTES)
  #define JPEG_DECODER_PIXEL_SIZE_IN_BYTES  DEFAULT_PIXEL_SIZE_IN_BYTES
#endif
/* --------- Stream management from FLASH buffer --------------------*/
extern const U8 *stream_jpeg_src_ptr;
extern U16 stream_src_size;
static size_t stream_offset;

void stream_open(void)
{
  stream_offset = 0;
}

size_t stream_read(JOCTET * buffer, size_t nb_byte)
{ // assume nb_byte never goes beyond the stream buffer size
	if ((stream_offset+nb_byte) > (size_t)stream_src_size)
	{
		nb_byte = (size_t)stream_src_size - stream_offset;
	}
	memcpy (buffer, (stream_jpeg_src_ptr + stream_offset), nb_byte);
	stream_offset += nb_byte;
	return nb_byte;
}

void stream_close()
{
}

void stream_seek(int offset)
{ // assume offset is always > 0 and inside the JPEG stream buffer
	stream_offset += offset;
}

/* ---------IJG JPEG library callback functions -------------------*/

/* Expanded data source object for stdio input */

typedef struct {
  struct jpeg_source_mgr pub;   /* public fields */

  FILE * infile;                /* source stream */
  JOCTET * buffer;              /* start of buffer */
  boolean start_of_file;        /* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;


 /* We use C's setjmp/longjmp facility to return control.  This means that the
 * routine which calls the JPEG library must first execute a setjmp() call to
 * establish the return point.  We want the replacement error_exit to do a
 * longjmp().  But we need to make the setjmp buffer accessible to the
 * error_exit routine.  To do this, we make a private extension of the
 * standard JPEG error handler object.  (If we were using C++, we'd say we
 * were making a subclass of the regular error handler.)
 *
 * Here's the extended error handler struct:
 */

struct extended_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct extended_error_mgr * extended_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
extended_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  extended_error_ptr extended_err = (extended_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(extended_err->setjmp_buffer, 1);
}

/* Callback function of the JPEG library in order to fill the input buffer.
 */
static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;
	size_t nbytes;

	nbytes = stream_read(src->buffer, JPEG_INPUT_BUF_SIZE);		//	nbytes = file_read_buf(src->buffer, JPEG_INPUT_BUF_SIZE);

	if(nbytes <= 0)
	{
		if (src->start_of_file)
		{ /* Treat empty input file as fatal error */
			return FALSE;//ERREXIT(cinfo, JERR_INPUT_EMPTY);
		}
		//WARNMS(cinfo, JWRN_JPEG_EOF);
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

/* Callback function of the JPEG library to skip data.
 */
static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}

/* JPEG library callback to initialize the input data source.
 */
static void init_source (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = TRUE;
}

/* JPEG library callback to do cleanup work after processing the input data.
 */
static void term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}

/* JPEG library callback to initialize this module.
 */
void jpeg_stdio_src (j_decompress_ptr cinfo, FILE * infile)
{
  my_src_ptr src;

  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling jpeg_stdio_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  if (cinfo->src == NULL) {     /* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  SIZEOF(my_source_mgr));
    src = (my_src_ptr) cinfo->src;
    // allocate input buffer
    src->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  JPEG_INPUT_BUF_SIZE * SIZEOF(JOCTET));
  }

  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->infile = infile;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}
/*----------------------------------------------------------------------*/

uint16_t *jpeg_out_buffer_pos;

void *jpeg_lib_decode_ex(int offset, U16 *width, U16 *height)
{
  struct jpeg_decompress_struct *cinfo = (struct jpeg_decompress_struct *)
    jpeg_lib_data.cinfo;

  cinfo->output_width = *width;
  cinfo->output_height = *height;

  if (!jpeg_lib_decode(offset))
    return NULL;

  *width = cinfo->output_width;
  *height = cinfo->output_height;

  return (void *) jpeg_lib_data.output_image;
}

bool  jpeg_lib_decode(int offset)
{
	struct jpeg_decompress_struct *cinfo = (struct jpeg_decompress_struct *) jpeg_lib_data.cinfo;
	struct extended_error_mgr *jerr = (struct extended_error_mgr *) cinfo->err;
	uint16_t max_lines = 1;
	uint16_t scale_denom;
	uint16_t max_width, max_height;

	max_width = cinfo->output_width;
    max_height = cinfo->output_height;

	// set output image position for the JPEG library
	jpeg_out_buffer_pos = (uint16_t *) jpeg_lib_data.output_image;

	//file_ptr = 0;
	stream_open();				//	file_open(FOPEN_MODE_R);
	if(offset)
		stream_seek(offset);	//	file_seek(offset, FS_SEEK_SET);

	// set file to read from
	jpeg_stdio_src(cinfo, 0);

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr->setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_abort_decompress(cinfo);
		stream_close();		//	file_close();
		return false;
	}
	// read the file header
	if(JPEG_HEADER_OK != jpeg_read_header(cinfo, TRUE))
	{
		jpeg_abort_decompress(cinfo);
		stream_close();		//	file_close();
		return false;
	}

	// set decompression configuration
	scale_denom = 0;

	// set correct scaling to fit the reserved space
	while(((cinfo->image_width >> scale_denom) > max_width)
		|| ((cinfo->image_height >> scale_denom) > max_height))
	{
		scale_denom++;
	}
	scale_denom = 1 << scale_denom;
	// any scaling above 1/8 is not possible
	if(scale_denom > 8)
	{
		stream_close();		//	file_close();
		return false;
	}

	cinfo->scale_denom = scale_denom;


	cinfo->dct_method = JDCT_FASTEST;
	cinfo->do_fancy_upsampling = false;
	cinfo->out_color_space = JCS_RGB565;

	// submit the requested decompression parameters
	// this call will also adjust invalid settings
	if( jpeg_start_decompress(cinfo) == false)
	{
		jpeg_abort_decompress(cinfo);
		stream_close();		//	file_close();
		return false;
	}

	// read scanlines
	//
	while(cinfo->output_scanline < cinfo->output_height)
	{
		// get decoded scanlines
		jpeg_read_scanlines(cinfo, NULL, max_lines);
	}

	jpeg_finish_decompress(cinfo);

	stream_close();		//	file_close();
	return true;
}

/* Call once before the start of any decompression
 */
bool jpeg_lib_init(void)
{
	// allocate the decompression structure
	jpeg_lib_data.cinfo = malloc(sizeof(struct jpeg_decompress_struct ));

	if(!jpeg_lib_data.cinfo)
		return false;

	jpeg_lib_data.cinfo->err = (struct jpeg_error_mgr *) malloc(sizeof(struct extended_error_mgr));

	if(!jpeg_lib_data.cinfo->err)
	{
		free(jpeg_lib_data.cinfo);
		return false;
	}
	// allocate the buffer for the decompressed image
	jpeg_lib_data.output_image = malloc(
		JPEG_DECODER_MAX_IMAGE_WIDTH * JPEG_DECODER_MAX_IMAGE_HEIGHT * JPEG_DECODER_PIXEL_SIZE_IN_BYTES);

	if(!jpeg_lib_data.output_image)
	{
		free(jpeg_lib_data.cinfo->err);
		free(jpeg_lib_data.cinfo);
		return false;
	}
	// initialize error handler
	jpeg_std_error(jpeg_lib_data.cinfo->err);
	jpeg_lib_data.cinfo->err->error_exit = extended_error_exit;

	// initialize the decompression struct
	jpeg_create_decompress(jpeg_lib_data.cinfo);

	return true;
}

void jpeg_lib_exit(void)
{
	if(jpeg_lib_data.output_image)
		free(jpeg_lib_data.output_image);

	if(jpeg_lib_data.cinfo->err)
		free(jpeg_lib_data.cinfo->err);

	if(jpeg_lib_data.cinfo)
	{
		// release internal memory
		jpeg_destroy_decompress(jpeg_lib_data.cinfo);
		free(jpeg_lib_data.cinfo);
	}
}
