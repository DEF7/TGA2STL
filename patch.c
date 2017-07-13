// patch.c - subdivision mesh algorithm (based on old-school 'ROAM' algorithm)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "math.h"
#include "patch.h"


#define MAX_PATCHNODES				(1 << 24)		// maximum binary-tree triangles


// I hate this hacky shizzles
struct tri;

//
typedef struct tri
{
	struct tri *child[2], *neigh[3];	// children and neighbor triangles
} tri_t;
//



// binary tree data
tri_t triangles[MAX_PATCHNODES];
unsigned int num_triangles = 0;

// meshified patch
pface_t patch_faces[MAX_PATCHFACES];
unsigned int num_patchfaces = 0;



//
tri_t *new_tri()
{
	tri_t *t;

	if(num_triangles == MAX_PATCHNODES)
		return 0;

	t = &triangles[num_triangles++];
	t->child[0] = 0;
	t->child[1] = 0;
	t->neigh[0] = 0;
	t->neigh[1] = 0;
	t->neigh[2] = 0;

	return t;
}
//

// update neighbor relationships between a triangle and its neighbors,
// along with maintaining the neighbor's of their children
int patch_settriangleneighbors(tri_t *t)
{
	t->child[0]->neigh[0] = t->child[1];
	t->child[0]->neigh[2] = t->neigh[0];

	t->child[1]->neigh[1] = t->child[0];
	t->child[1]->neigh[2] = t->neigh[1];

	if(t->neigh[0])		// if any of left-neighbor's neighbors are self, set to our left child
	{
		if(t->neigh[0]->neigh[2] == t)
			t->neigh[0]->neigh[2] = t->child[0];
		else if(t->neigh[0]->neigh[0] == t)
			t->neigh[0]->neigh[0] = t->child[0];
		else if(t->neigh[0]->neigh[1] == t)
			t->neigh[0]->neigh[1] = t->child[0];
	}

	if(t->neigh[1])		// if any of right-neighbor's neighbors are self, set to our right child
	{
		if(t->neigh[1]->neigh[2] == t)
			t->neigh[1]->neigh[2] = t->child[1];
		else if(t->neigh[1]->neigh[1] == t)
			t->neigh[1]->neigh[1] = t->child[1];
		else if(t->neigh[1]->neigh[0] == t)
			t->neigh[1]->neigh[0] = t->child[1];
	}

	if(t->neigh[2])		// base neighbor
	{
		if(t->neigh[2]->child[0])		// set neighbors for baseneighbor's children
		{
			t->neigh[2]->child[0]->neigh[1] = t->child[1];
			t->neigh[2]->child[1]->neigh[0] = t->child[0];
			t->child[0]->neigh[1] = t->neigh[2]->child[1];
			t->child[1]->neigh[0] = t->neigh[2]->child[0];
		}
		else							// baseneighbor has no children
			return 1;
	}
	else
	{
		t->child[0]->neigh[1] = 0;
		t->child[1]->neigh[0] = 0;
	}

	return 0;
}
//

// divide a triangle at its hypoteneuse along with its baseneighbor to prevent cracks
void patch_splittriangle(tri_t *t)
{
	if(t->child[0])
		return;

	if(t->neigh[2])
		if(t->neigh[2]->neigh[2] != t)				// if baseneighbor's baseneighbor isn't us, we need to split them first
				patch_splittriangle(t->neigh[2]);

	// split triangle by creating child triangles 
	if(!(t->child[0] = new_tri()) || !(t->child[1] = new_tri()))
	{
		print("patch_splittriangle: binary tree node overflow!");
		return;
	}

	// update neighbor assignments
	if(patch_settriangleneighbors(t))
			patch_splittriangle(t->neigh[2]);		// forcesplit our base neighbor to prevent cracks, if it's not already split
}
//

// quick macro for accessing heightmap via XY coordinate
#define hmap(x, y)		heights[((unsigned int)x + (unsigned int)y * width)]

// recursively subdivide triangles until we are down to single pixels
// or until the triangle conforms to the heightmap within a tolerance
void patch_recursedivide(tri_t *t, float *heights, int width, int height, float thresh, vec3 left, vec3 apex, vec3 right, float level)
{
	vec3 pt, delta = (vec3){ fabs(left.x - right.x), fabs(left.y - right.y), 0 };
	float z;

	// make sure we're not down smaller than heightmap grid
	if(((delta.x && delta.x <= 1) || (delta.y && delta.y <= 1) || (!delta.x && !delta.y)))
		return;

	pt = vmix(left, right, 0.5);
	z = fabs(pt.z - hmap(pt.x, pt.y));

	// split if it has no children and deviates from heightmap enough
	if(!t->child[0] && (level <= 3 || z >= thresh * level))
		patch_splittriangle(t);

	// recurse down through children, if we have any to deal with
	if(t->child[0])
	{
		// get center of hypoteneuse
		pt = vmix(left, right, 0.5);
		// use heightmap's z as childrens' apex vertex z
		pt.z = hmap(pt.x, pt.y);

		level = sqrt(level * level + 1);
		patch_recursedivide(t->child[0], heights, width, height, thresh, apex, pt, left, level);
		patch_recursedivide(t->child[1], heights, width, height, thresh, right, pt, apex, level);
	}
}
//

// recursively subdivide triangles until we are down to single pixels
// or until the triangle conforms to the heightmap within a tolerance
void patch_recursemesh(tri_t *t, float *heights, int width, int height, vec3 left, vec3 apex, vec3 right)
{
	pface_t pf;
	vec3 pt;

	// calculate hypoteneuse center point by averaging endpoints together
	pt = vscale(vadd(left, right), 0.5);
	// get actual heightmap value for this horizontal position
	pt.z = hmap(pt.x, pt.y);

	// recurse subdivide
	if(t->child[0])
	{
		patch_recursemesh(t->child[0], heights, width, height, apex, pt, left);
		patch_recursemesh(t->child[1], heights, width, height, right, pt, apex);
	}
	else	// leaf triangle, output its geometry
	{
		if(num_patchfaces == MAX_PATCHFACES)
		{
			print("patch_recursemesh: triangle face overflow!");
			return;
		}

		pf.verts[0] = left;
		pf.verts[1] = apex;
		pf.verts[2] = right;
		patch_faces[num_patchfaces++] = pf;
	}
}
//

// generate a mesh that conforms to a floating-point heightmap
// using subdividing triangles similar to ROAM terrain algorithm
void patch_meshheightmap(float *heights, int width, int height, float hscale, float vscale, float thresh)
{
	int x;
	pface_t *p;
	vec3 pts[] =		// outer edge of heightmap, origin triangles
	{
		{ 0, 0, hmap(0, 0) },
		{ width - 1, 0, hmap(width - 1, 0) },
		{ width - 1, height - 1, hmap(width - 1, height - 1) },
		{ 0, height - 1, hmap(0, height - 1) }
	};

	// initialize root triangles
	num_triangles = 2;
	triangles[0].neigh[2] = &triangles[1];		// set as eachothers' base neighbors
	triangles[1].neigh[2] = &triangles[0];
	
	// recursively subdivide to conform triangles to heightmap within a tolerance
	patch_recursedivide(&triangles[0], heights, width, height, thresh, pts[0], pts[1], pts[2], 1.0f);
	patch_recursedivide(&triangles[1], heights, width, height, thresh, pts[2], pts[3], pts[0], 1.0f);

	print("...generated %d binary tree triangles", num_triangles);

	// recursively subdivide to iterate through the triangle tree, outputting triangle faces
	patch_recursemesh(&triangles[0], heights, width, height, pts[0], pts[1], pts[2]);
	patch_recursemesh(&triangles[1], heights, width, height, pts[2], pts[3], pts[0]);

	print("...generated %d mesh triangle faces", num_patchfaces);

	// scale mesh vertices to user settings
	for(x = 0; x < num_patchfaces; x++)
	{
		p = &patch_faces[x];
		p->verts[0].x *= hscale;
		p->verts[0].y *= hscale;
		p->verts[0].z *= vscale;
		p->verts[1].x *= hscale;
		p->verts[1].y *= hscale;
		p->verts[1].z *= vscale;
		p->verts[2].x *= hscale;
		p->verts[2].y *= hscale;
		p->verts[2].z *= vscale;
	}

	print("...scaled triangles");
}
//


