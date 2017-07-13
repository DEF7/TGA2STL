#ifndef H_STL
#define H_STL


#define MAX_STLDATASIZE			(1 << 26)		// 64mb max


// STL file data
extern unsigned int stl_datasize;
extern unsigned char stl_data[MAX_STLDATASIZE];


// add a new face to the STL mesh
void stl_new(char *header);
void stl_face(vec3 p1, vec3 p2, vec3 p3);
void stl_end();

#endif
