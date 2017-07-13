#ifndef H_PATCH
#define H_PATCH


#define MAX_PATCHFACES				(1 << 24)		// maximum mesh triangles output

//
typedef struct
{
	vec3 verts[3];
} pface_t;
//


//
extern pface_t patch_faces[MAX_PATCHFACES];
extern unsigned int num_patchfaces;

//
void patch_meshheightmap(float *heights, int width, int height, float hscale, float vscale, float thresh);


#endif
