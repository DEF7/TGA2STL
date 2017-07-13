#ifndef H_MATH
#define H_MATH


#define DEG2RAD		(3.141592654f / 180)
#define EPSILON		0.0000001

#define MAX(a, b)		(a > b ? a : b)
#define MIN(a, b)		(a < b ? a : b)

#define ROUND(a)		( ((a) >= 0) ? (a + 0.5) : (a - 0.5) )

#define FLERP(a, b, z)		((a) * (1.0f - z) + (b) * (z))


//
typedef struct
{
	float	xx, xy, xz, xw,		// x column
			yx, yy, yz, yw,		// y	"
			zx, zy, zz, zw,		// z	"
			wx, wy, wz, ww;		// w	"
} mat4;
//

//
typedef struct
{
	float x, y, z;
} vec3;
//

//
typedef struct
{
	float x, y, z, w;
} vec4;
//


float fpow(float f, float e);
float fsqrt(float x);
float finvsqrt(float x);
float fwrap(float x, float min, float max);
float fclamp(float x, float min, float max);

vec3 specular_hyperbola(float azimuth, float depth, float light_angle);

vec3 vadd(vec3 a, vec3 b);
vec3 vsub(vec3 a, vec3 b);
vec3 vmul(vec3 a, vec3 b);
vec3 vscale(vec3 a, float scale);
vec3 vmix(vec3 a, vec3 b, float t);
vec3 vcross(vec3 a, vec3 b);
float vdot(vec3 a, vec3 b);
float vlensqr(vec3 a);
float vlen(vec3 a);
float vdist(vec3 a, vec3 b);
vec3 vnorm(vec3 a);
vec3 vnormp(vec3 a);

//
mat4 mrot(float angle, float x, float y, float z);
mat4 minverse(mat4 m);
mat4 mtranspose(mat4 m);
mat4 mmul(mat4 a, mat4 b);
vec3 vec3mat4(vec3 v, mat4 m);

vec3 ray_distance(vec3 a, vec3 b, vec3 c, float *frac);
int intersect_ray_cube(vec3 a, vec3 b, vec3 org, float radius);
int intersect_cube_triangle(vec3 org, float radius, vec3 v1, vec3 v2, vec3 v3);
float intersect_ray_triangle(vec3 a, vec3 b, vec3 v1, vec3 v2, vec3 v3);

#endif
