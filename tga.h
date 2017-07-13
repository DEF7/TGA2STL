#ifndef H_TGA
#define H_TGA

// TGA defines
#define MAX_TGADIMENSION			2048			// max width / height
#define MAX_TGACHANNELS				4
#define MAX_TGABYTES				(MAX_TGADIMENSION * MAX_TGADIMENSION * MAX_TGACHANNELS)


// globals
extern int tga_width, tga_height, tga_channels, tga_size;
extern unsigned char tga_data[MAX_TGABYTES];


//
int tga_getpixels(void *data);


#endif
