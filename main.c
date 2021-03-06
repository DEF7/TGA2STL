// main.c - converts a TGA file into an STL mesh

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "math.h"
#include "cvar.h"
#include "tga.h"
#include "patch.h"
#include "stl.h"


#define MAX_HEIGHTMAPSIZE		2048

// argument defaults
#define DEFAULT_SMOOTHS			2
#define DEFAULT_THRESH			0.1
#define DEFAULT_HSCALE			128			// 128 pixels/inch
#define DEFAULT_VSCALE			256			// 256 shades/inch



// array of heightmap values
float heightmap[MAX_HEIGHTMAPSIZE * MAX_HEIGHTMAPSIZE];
// program argument variables
cvar_t *var_smooths, *var_thresh, *var_hscale, *var_vscale;



// get the time
double time_query()
{
	return (double)clock() / CLOCKS_PER_SEC;		// todo: use a system function for better precision ?
}
//

// wrapper log/console print
void print(char *str, ...)
{
	va_list argptr;
	char text[1024];
	int sz;
	FILE *log;

	text[0] = 0;

	// do it
	va_start(argptr, str);
	vsprintf(text, str, argptr);
	va_end(argptr);

	// append to log file directly
	if((log = fopen(APP_LOG, "at")))
	{
		sz = fprintf(log, "%05.3f %s\n", time_query(), text);
		fclose(log);
	}

	// output to console window, sans timestamp
	printf("%s\n", text);
}
//

//
void *file_load(char *path)
{
	unsigned char *data;
	FILE *f;
	int size;

	if(!(f = fopen(path, "rb")))
		return 0;

	// get file size
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	// allocate buffer
	data = (unsigned char *)malloc(size);

	// read + close file
	fread(data, size, 1, f);
	fclose(f);

	return (void *)data;
}
//

//
int file_save(char *path, void *data, unsigned int size)
{
	FILE *f;

	if(!(f = fopen(path, "wb")))
		return 0;

	fwrite(data, size, 1, f);
	fclose(f);
	return 1;
}
//


// averages all color channels together, smooths resulting heightmap (wrap-around smoothing)
void pixels_to_heightmap(unsigned char *pixels, int width, int height, int channels)
{
	float v, *buffer;	// buffer for proper smoothang
	int x, y, z, sz_heightmap = sizeof(float) * width * height;

	#define hbuf(a, b)		buffer[ ((a) + (b) * width) ]
	#define hmap(a, b)		heightmap[ ((a) + (b) * width) ]

	buffer = (float *)malloc(sz_heightmap);

	// average together all channels into heightmap value
	for(y = 0; y < height; y++)
	for(x = 0; x < width; x++)
	{
		for(v = pixels[0], z = 1; z < channels; z++)
			v += pixels[z];

		hmap(x, y) = v / channels;
		pixels += channels;
	}

	// smoothing
	for(z = 0; z < var_smooths->value.i; z++)
	{
		// copy heightmap over to buffer, so we can overwrite the heightmap without
		// the issues that occur with naive in-place smoothing
		memcpy(buffer, heightmap, sz_heightmap);

		for(y = 1; y < height - 1; y++)
		for(x = 1; x < width - 1; x++)
		{
			hmap(x, y) =
			(
				hbuf(x - 1, y - 1) + hbuf(x, y - 1) + hbuf(x + 1, y - 1) +
				hbuf(x - 1, y) + hbuf(x, y) + hbuf(x + 1, y) +
				hbuf(x - 1, y + 1) + hbuf(x, y + 1) + hbuf(x + 1, y + 1)
			) / 9;
		}
	}

	// done
	free((void *)buffer);
}
//


//
int main(int argc, char **argv)
{
	int x;
	void *tga;
	char outpath[256];

	remove(APP_LOG);

	//
	print("%s - %s\n%s\n%s\n", APP_NAME, APP_DATE, APP_AUTH, APP_SITE);

	// generate argument cvars
	var_thresh = cvar_newfloat("thresh", DEFAULT_THRESH);
	var_smooths = cvar_newint("smooths", DEFAULT_SMOOTHS);
	var_hscale = cvar_newint("hscale", DEFAULT_HSCALE);
	var_vscale = cvar_newint("vscale", DEFAULT_VSCALE);

	// do we have at least a filepath argument?
	if(argc < 2)
	{
		print("usage: tga2stl <tgapath> [-thresh #.#] [-smooths #] [-hscale #] [-vscale #]\n");
		print(" thresh: maximum mesh/heightmap error (default = %2.2f)", DEFAULT_THRESH);
		print(" smooths: smoothing passes on heightmap data before meshing. (default = %d)", DEFAULT_SMOOTHS);
		print(" hscale: horizontal scale, pixels-per-inch. (default = %d)", DEFAULT_HSCALE);
		print(" vscale: vertical scaling, image gradiations-per-inch. (default = %d)", DEFAULT_VSCALE);
		print("");
		system("PAUSE");
		return 0;
	}

	// get any other arguments
	for(x = 2; x < argc; x += 2)
	{
		if(argv[x][0] != '-')
			continue;

		// find and set value of cvar, extracting value
		// from value string based on the cvar's type
		cvar_setvalue(argv[x] + 1, argv[x + 1]);
	}

	//
	print("loading \"%s\"...", argv[1]);

	if(!(tga = file_load(argv[1])))
	{
		print("...unable to open \"%s\" for reading", argv[1]);
		return 0;
	}

	// convert tga file to raw pixel data
	if(tga_getpixels(tga))
	{	
		// convert pixels to heightmap values
		print("generating heightmap data from pixel data...");
		pixels_to_heightmap((unsigned char *)tga_data, tga_width, tga_height, tga_channels);

		// generate data structure of optimized triangular representation
		print("generating triangle mesh...");
		patch_meshheightmap(heightmap, tga_width, tga_height, 1.0f / var_hscale->value.i, 1.0f / var_vscale->value.i, var_thresh->value.f);

		// output STL model file
		print("generating STL model data...");
		stl_new("model generated by TGA2STL");

		// output patch faces to STL data..
		for(x = 0; x < num_patchfaces; x++)
			stl_face(patch_faces[x].verts[0], patch_faces[x].verts[1], patch_faces[x].verts[2]);

		stl_end();

		// generate .stl path from .tga path
		sprintf(outpath, "%s", argv[1]);
		x = strlen(outpath);
		sprintf(&outpath[x - 3], "stl");

		print("saving \"%s\"...", outpath);
		file_save(outpath, (void *)stl_data, stl_datasize);

		print("...done");
	}

	// free image data
	free(tga);

	print("");
	system("PAUSE");
	return 0;
}
//

