// tga.c - loading TGA images

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "tga.h"


// TGA header structure
typedef struct
{
	unsigned char id_length;
	unsigned char colormap_type;
	unsigned char type;
	unsigned char colormap_spec[5];
	unsigned short origin_x;
	unsigned short origin_y;
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char descriptor;
} tgaheader_t;
//


// sizes
int sz_tgaheader = sizeof(tgaheader_t);

// globals
int tga_width, tga_height, tga_channels, tga_size;
unsigned char tga_data[MAX_TGABYTES];



//
int tga_getpixels(void *data)
{
	tgaheader_t *image = (tgaheader_t *)data;
	unsigned char swap, *pixels;
	int c, rowsize;

	// check that TGA is raw bitmap
	if(image->id_length || image->colormap_type || (image->type != 2))
	{
		print("tga_getpixels: invalid targa format");
		return 0;
	}

	// extract image specs
	tga_width = image->width;
	tga_height = image->height;
	tga_channels = image->bpp >> 3;		// divide bits by eight for bytes/channels

	// validate image specs
	if(!tga_width || !tga_height || (tga_channels != 3 && tga_channels != 4))
	{
		print("tga_getpixels: invalid targa dimensions");
		return 0;
	}

	// determine size of image pixel data
	tga_size = tga_width * tga_height * tga_channels;

	// get pointer to pixel data
	pixels = (unsigned char *)data + sz_tgaheader;

	// check if origin is bottom-left or top-left (5th bit of descriptor byte)
	if(image->descriptor & 32)
	{
		print("tga_getpixels: copying bottom-left origin pixel data");
		rowsize = tga_width * tga_channels;		// bytes per row of pixels

		for(c = 0; c < tga_height; c++)		// copy pixel rows in reverse order
			memcpy(tga_data + rowsize * c, pixels + rowsize * (tga_height - (c + 1)), rowsize);
	}
	else
	{
		print("tga_getpixels: copying top-left origin pixel data");
		memcpy(tga_data, pixels, tga_size);
	}

	// swap red and blue bytes (TGA is funky like that)
	for(c = 0; c < tga_size; c += tga_channels)
	{
		swap = tga_data[c];
		tga_data[c] = tga_data[c + 2];
		tga_data[c + 2] = swap;
	}

	print("tga_pixels: extracted %dx%dx%d bytes of pixel data from targa", tga_width, tga_height, tga_channels);
	return tga_size;
}
//
