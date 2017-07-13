// stl.c - stereolithography mesh stuff

#include <stdio.h>

#include "math.h"
#include "stl.h"


//
typedef struct
{
	vec3 normal, p1, p2, p3;
	unsigned short aux;
} stlface_t;
//


unsigned int stl_datasize, num_stlfaces;
unsigned char stl_data[MAX_STLDATASIZE];

unsigned int *stl_numtrisptr = (unsigned int *)&stl_data[80];


//
void stl_new(char *header)
{
	sprintf((char *)stl_data, "[ %s ]", header);
	stl_datasize = 84;
	num_stlfaces = 0;
}
//

// generate face normal, write face data to stl_data
void stl_face(vec3 p1, vec3 p2, vec3 p3)
{
	stlface_t *face;

	if(stl_datasize + 50 > MAX_STLDATASIZE)
	{
		print("stl_face: STL buffer overflow!");
		return;
	}

	face = (stlface_t *)&stl_data[stl_datasize];
	stl_datasize += 50;		// unpadded size of stlface_t

	// calculate normal
	face->normal = vnormp(vcross(vsub(p1, p2), vsub(p3, p2)));
	// write vertices
	face->p1 = p1;
	face->p2 = p2;
	face->p3 = p3;
	// etc..
	face->aux = 0;

	num_stlfaces++;
}
//

//
void stl_end()
{
	*stl_numtrisptr = num_stlfaces;
}
//
