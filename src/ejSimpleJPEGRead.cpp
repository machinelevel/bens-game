/**** ej's simple jpeg read code ****/

#include <stdio.h>
#include <malloc.h>

extern "C" {
 #include "jpeglib.h"
}

static unsigned char *ramJpegData = NULL;
static int ramJpegSize = 0;
extern jpeg_source_mgr ramsrc;

void ram_init_source(j_decompress_ptr cinfo) {}

boolean ram_fill_input_buffer(j_decompress_ptr cinfo) {
	return(true);
}

void ram_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	ramsrc.next_input_byte += num_bytes;
	ramsrc.bytes_in_buffer -= num_bytes;
}

boolean ram_resync_to_restart(j_decompress_ptr cinfo, int desired)
{
	ramsrc.next_input_byte = ramJpegData;
	ramsrc.bytes_in_buffer = ramJpegSize;
	return(true);
}

void ram_term_source(j_decompress_ptr cinfo) {}

jpeg_source_mgr ramsrc = {
  NULL, //const JOCTET * next_input_byte; /* => next byte to read from buffer */
  0, //size_t bytes_in_buffer;	/* # of bytes remaining in buffer */

  ram_init_source, //JMETHOD(void, init_source, (j_decompress_ptr cinfo));
  ram_fill_input_buffer, //JMETHOD(boolean, fill_input_buffer, (j_decompress_ptr cinfo));
  ram_skip_input_data, //JMETHOD(void, skip_input_data, (j_decompress_ptr cinfo, long num_bytes));
  ram_resync_to_restart, //JMETHOD(boolean, resync_to_restart, (j_decompress_ptr cinfo, int desired));
  ram_term_source, //JMETHOD(void, term_source, (j_decompress_ptr cinfo));
};

void jpeg_ram_src(jpeg_decompress_struct *cinfo, void *mem, int size)
{
	ramsrc.next_input_byte = ramJpegData = (unsigned char *)mem;
	ramsrc.bytes_in_buffer = ramJpegSize = size;
	cinfo->src = &ramsrc;
}

//extern "C" void __declspec ( naked ) _chkesp(void)
//{
//	_asm { ret }
//}

bool ejSimpleJPEGRead(FILE *inputFile, void *inputMem, int inputMemSize, void **pOutData, unsigned int *pOutWidth, unsigned int *pOutHeight, unsigned int *pOutBPP)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int		i, j, width, height, lineCount;
	void	*buffer;
	unsigned char	**rows, *row1, *row2, pix;

	if (pOutWidth)	*pOutWidth = 0;
	if (pOutHeight)	*pOutHeight = 0;
	if (pOutBPP)	*pOutBPP = 0;
	if (pOutData)	*pOutData = NULL;

	if (!inputFile && !inputMem) return(false);

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	if (inputFile) jpeg_stdio_src(&cinfo, inputFile);
	else jpeg_ram_src(&cinfo, inputMem, inputMemSize);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	width = cinfo.output_width;
	height = cinfo.output_height;
	buffer = (void*)malloc(width * height * 4);
	rows = (unsigned char **)malloc(height * sizeof(unsigned char *));
	for (i = 0; i < height; i++) {
		rows[i] = (unsigned char *)(((char*)buffer) + (3 * width * i));
	}
	lineCount = 0;
	if (buffer && rows) {
		while (lineCount < height) {
			lineCount += jpeg_read_scanlines(&cinfo, &(rows[lineCount]), height);
		}
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	/**** now, must flip it vertically for some silly reason. ****/
	for (i = 0; i < height/2; i++) {
		row1 = rows[i];
		row2 = rows[height - (i + 1)];
		for (j = 0; j < width * 3; j++) {
			pix = *row1;
			*row1++ = *row2;
			*row2++ = pix;
		}
	}

	if (!buffer) return(false);
	if (pOutWidth)	*pOutWidth = width;
	if (pOutHeight)	*pOutHeight = height;
	if (pOutBPP)	*pOutBPP = 24;
	if (pOutData)	*pOutData = buffer;
	return(true);
}
