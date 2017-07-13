// math.c - various math stuffs

#include <math.h>

#include "main.h"
#include "math.h"


//
float fpow(float f, float e)
{
	return pow(f, e);
}
//

// square root
float fsqrt(float x)
{
	union { float f; int i; } u;

//	if(!x)
//		return 0;

	u.f = x;
	u.i = 0x5F3759DF - (u.i >> 1);
	return x * u.f * (1.5f - x * u.f * u.f * 0.5f);
}
//

// inverse square root
float finvsqrt(float x)
{
	union { float f; int i; } u;

	if(!x)
		return 9999999;

	u.f = x;
	u.i = 0x5F3759DF - (u.i >> 1);
    return u.f * (1.5f - x * u.f * u.f * 0.5f);
}
//

//
float fwrap(float x, float min, float max)
{
	float delta = max - min, ret = fmodf((x - min), delta) + min;

	if(ret < min)	// mod doesn't do the job for us entirely sometimes
		ret += delta;

	return ret;
}
//

//
float fclamp(float x, float min, float max)
{
	return MIN(MAX(x, min), max);
}
//

// 
vec3 specular_hyperbola(float azimuth, float depth, float light_angle)
{
	vec3 ret = { 0, 0, 0 };

	// per Matt Brand via Specular Holography whitepaper
	ret.x = sin(azimuth * DEG2RAD) * -depth;
	ret.y = depth / cos(light_angle * DEG2RAD) * sqrt((ret.x * ret.x) / (depth * depth) + 1);
	ret.x *= 0.5;

	// return XY of an azimuthal point on hyperbola, depending on azimuth and light altitude
	return ret;
}
//


//
vec3 vadd(vec3 a, vec3 b)
{
	return (vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}
//

//
vec3 vsub(vec3 a, vec3 b)
{
	return (vec3){ a.x - b.x, a.y - b.y, a.z - b.z };
}
//

//
vec3 vmul(vec3 a, vec3 b)
{
	return (vec3){ a.x * b.x, a.y * b.y, a.z * b.z };
}
//

//
vec3 vscale(vec3 a, float scale)
{
	return (vec3){ a.x * scale, a.y * scale, a.z * scale };
}
//

//
vec3 vmix(vec3 a, vec3 b, float t)
{
	return vadd(vscale(a, 1.0f - t), vscale(b, t));
}
//

//
vec3 vcross(vec3 a, vec3 b)
{
	return (vec3){ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}
//

//
float vdot(vec3 a, vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}
//

// 
float vlensqr(vec3 a)
{
	return vdot(a, a);
}
//

// crude vector length
float vlen(vec3 a)
{
	return fsqrt(vdot(a, a));
}
//

// distance between two vectors
float vdist(vec3 a, vec3 b)
{
	return vlen(vsub(a, b));
}
//

// fast/crude normalize
vec3 vnorm(vec3 a)
{
	return vscale(a, finvsqrt(vlensqr(a)));
}
//

// precise normalize
vec3 vnormp(vec3 a)
{
	return vscale(a, 1.0f / sqrt(vlensqr(a)));
}
//



// rotation matrix of 'angle' degrees around axis vector 'xyz'
mat4 mrot(float angle, float x, float y, float z)
{
	float x2 = x * x, y2 = y * y, z2 = z * z;
	float ang = angle * -DEG2RAD;		// left-handed
	float c = cos(ang), s = sin(ang), omc = 1.0f - c;
	mat4 ret;

	ret.xx = x2 * omc + c;
	ret.xy = x * y * omc + z * s;
	ret.xz = x * z * omc - y * s;
	ret.xw = 0;

	ret.yx = x * y * omc - z * s;
	ret.yy = y2 * omc + c;
	ret.yz = y * z * omc + x * s;
	ret.yw = 0;

	ret.zx = x * z * omc + y * s;
	ret.zy = y * z * omc - x * s;
	ret.zz = z2 * omc + c;
	ret.zw = 0;

	ret.wx = 0;
	ret.wy = 0;
	ret.wz = 0;
	ret.ww = 1;

	return ret;
}
//

//
mat4 minverse(mat4 m)
{
    float det;
	mat4 ret;

	ret.xx = m.yy  * m.zz * m.ww - m.yy  * m.zw * m.wz - 
		m.zy  * m.yz  * m.ww + m.zy  * m.yw  * m.wz +
		m.wy * m.yz  * m.zw - m.wy * m.yw  * m.zz;

	ret.xy = -m.xy  * m.zz * m.ww + m.xy  * m.zw * m.wz + 
		m.zy  * m.xz * m.ww - m.zy  * m.xw * m.wz - 
		m.wy * m.xz * m.zw + m.wy * m.xw * m.zz;

	ret.xz = m.xy  * m.yz * m.ww - m.xy  * m.yw * m.wz - 
		m.yy  * m.xz * m.ww + m.yy  * m.xw * m.wz + 
		m.wy * m.xz * m.yw - m.wy * m.xw * m.yz;

	ret.xw = -m.xy * m.yz * m.zw + m.xy * m.yw * m.zz + 
		m.yy * m.xz * m.zw - m.yy * m.xw * m.zz - 
		m.zy * m.xz * m.yw + m.zy * m.xw * m.yz;

	ret.yx = -m.yx  * m.zz * m.ww + m.yx  * m.zw * m.wz + 
		m.zx  * m.yz  * m.ww - m.zx  * m.yw  * m.wz - 
		m.wx * m.yz  * m.zw + m.wx * m.yw  * m.zz;

	ret.yy = m.xx  * m.zz * m.ww - m.xx  * m.zw * m.wz - 
		m.zx  * m.xz * m.ww + m.zx  * m.xw * m.wz + 
		m.wx * m.xz * m.zw - m.wx * m.xw * m.zz;

	ret.yz = -m.xx  * m.yz * m.ww + m.xx  * m.yw * m.wz + 
		m.yx  * m.xz * m.ww - m.yx  * m.xw * m.wz - 
		m.wx * m.xz * m.yw + m.wx * m.xw * m.yz;

	ret.yw = m.xx * m.yz * m.zw - m.xx * m.yw * m.zz - 
		m.yx * m.xz * m.zw + m.yx * m.xw * m.zz + 
		m.zx * m.xz * m.yw - m.zx * m.xw * m.yz;

	ret.zx = m.yx  * m.zy * m.ww - m.yx  * m.zw * m.wy - 
		m.zx  * m.yy * m.ww + m.zx  * m.yw * m.wy + 
		m.wx * m.yy * m.zw - m.wx * m.yw * m.zy;

	ret.zy = -m.xx  * m.zy * m.ww + m.xx  * m.zw * m.wy + 
		m.zx  * m.xy * m.ww - m.zx  * m.xw * m.wy - 
		m.wx * m.xy * m.zw + m.wx * m.xw * m.zy;	

	ret.zz = m.xx  * m.yy * m.ww - m.xx  * m.yw * m.wy - 
		m.yx  * m.xy * m.ww + m.yx  * m.xw * m.wy + 
		m.wx * m.xy * m.yw - m.wx * m.xw * m.yy;

	ret.zw = -m.xx * m.yy * m.zw + m.xx * m.yw * m.zy + 
		m.yx * m.xy * m.zw - m.yx * m.xw * m.zy - 
		m.zx * m.xy * m.yw + m.zx * m.xw * m.yy;

	ret.wx = -m.yx  * m.zy * m.wz + m.yx  * m.zz * m.wy +
		m.zx  * m.yy * m.wz - m.zx  * m.yz * m.wy - 
		m.wx * m.yy * m.zz + m.wx * m.yz * m.zy;

	ret.wy = m.xx  * m.zy * m.wz - m.xx  * m.zz * m.wy - 
		m.zx  * m.xy * m.wz + m.zx  * m.xz * m.wy + 
		m.wx * m.xy * m.zz - m.wx * m.xz * m.zy;

	ret.wz = -m.xx  * m.yy * m.wz + m.xx  * m.yz * m.wy + 
		m.yx  * m.xy * m.wz - m.yx  * m.xz * m.wy - 
		m.wx * m.xy * m.yz + m.wx * m.xz * m.yy;

	ret.ww = m.xx * m.yy * m.zz - m.xx * m.yz * m.zy - 
		m.yx * m.xy * m.zz + m.yx * m.xz * m.zy + 
		m.zx * m.xy * m.yz - m.zx * m.xz * m.yy;

	det = m.xx * ret.xx + m.xy * ret.yx + m.xz * ret.zx + m.xw * ret.wx;

	if (det == 0)
		return ret;

	det = 1.0 / det;
	ret.xx *= det;
	ret.xy *= det;
	ret.xz *= det;
	ret.xw *= det;
	ret.yx *= det;
	ret.yy *= det;
	ret.yz *= det;
	ret.yw *= det;
	ret.zx *= det;
	ret.zy *= det;
	ret.zz *= det;
	ret.zw *= det;
	ret.wx *= det;
	ret.wy *= det;
	ret.wz *= det;
	ret.ww *= det;

	return ret;
}
//

// transpose matrix
mat4 mtranspose(mat4 m)
{
	mat4 ret;

	ret.xx = m.xx;
	ret.xy = m.yx;
	ret.xz = m.zx;
	ret.xw = m.wx;
	ret.yx = m.xy;
	ret.yy = m.yy;
	ret.yz = m.zy;
	ret.yw = m.wy;
	ret.zx = m.xz;
	ret.zy = m.yz;
	ret.zz = m.zz;
	ret.zw = m.wz;
	ret.wx = m.xw;
	ret.wy = m.yw;
	ret.wz = m.zw;
	ret.ww = m.ww;

	return ret;
}
//

// matrix multiplication
mat4 mmul(mat4 a, mat4 b)
{
	mat4 ret;

	// x column
	ret.xx = a.xx * b.xx + a.yx * b.xy + a.zx * b.xz + a.wx * b.xw;
	ret.xy = a.xy * b.xx + a.yy * b.xy + a.zy * b.xz + a.wy * b.xw;
	ret.xz = a.xz * b.xx + a.yz * b.xy + a.zz * b.xz + a.wz * b.xw;
	ret.xw = a.xw * b.xx + a.yw * b.xy + a.zw * b.xz + a.ww * b.xw;

	// y column
	ret.yx = a.xx * b.yx + a.yx * b.yy + a.zx * b.yz + a.wx * b.yw;
	ret.yy = a.xy * b.yx + a.yy * b.yy + a.zy * b.yz + a.wy * b.yw;
	ret.yz = a.xz * b.yx + a.yz * b.yy + a.zz * b.yz + a.wz * b.yw;
	ret.yw = a.xw * b.yx + a.yw * b.yy + a.zw * b.yz + a.ww * b.yw;

	// z column
	ret.zx = a.xx * b.zx + a.yx * b.zy + a.zx * b.zz + a.wx * b.zw;
	ret.zy = a.xy * b.zx + a.yy * b.zy + a.zy * b.zz + a.wy * b.zw;
	ret.zz = a.xz * b.zx + a.yz * b.zy + a.zz * b.zz + a.wz * b.zw;
	ret.zw = a.xw * b.zx + a.yw * b.zy + a.zw * b.zz + a.ww * b.zw;

	// w column
	ret.wx = a.xx * b.wx + a.yx * b.wy + a.zx * b.wz + a.wx * b.ww;
	ret.wy = a.xy * b.wx + a.yy * b.wy + a.zy * b.wz + a.wy * b.ww;
	ret.wz = a.xz * b.wx + a.yz * b.wy + a.zz * b.wz + a.wz * b.ww;
	ret.ww = a.xw * b.wx + a.yw * b.wy + a.zw * b.wz + a.ww * b.ww;

	return ret;
}
//

// multiply vec3 by mat4
vec3 vec3mat4(vec3 v, mat4 m)
{
	vec3 ret;
	ret.x = m.xx * v.x + m.yx * v.y + m.zx * v.z + m.wx;
	ret.y = m.xy * v.x + m.yy * v.y + m.zy * v.z + m.wy;
	ret.z = m.xz * v.x + m.yz * v.y + m.zz * v.z + m.wz;
	return ret;
}
//


// find point on line seg 'ab' that is nearest point 'c'
vec3 ray_distance(vec3 a, vec3 b, vec3 c, float *frac)
{
	vec3 line = vsub(b, a), delta = vsub(c, a);
	float f = fclamp(vdot(line, delta) / vlen(line), 0, 1);

	// return what fraction of line 'ab' the point lies at
	if(frac)
		*frac = f;

	// return calculated point on line
	return vadd(a, vscale(line, f));
}
//

//
int point_sameside(vec3 p1, vec3 p2, vec3 a, vec3 b)
{
	vec3 cp1, cp2;

	cp1 = vnorm(vcross(vsub(b, a), vsub(p1, a)));
	cp2 = vnorm(vcross(vsub(b, a), vsub(p2, a)));

	// vectors are perpendicular or converging
	if(vdot(cp1, cp2) >= -EPSILON)
		return 1;

	return 0;
}
//


//
int intersect_ray_cube(vec3 a, vec3 b, vec3 org, float radius)
{
	vec3 delta;
	float dist, rad;

	// get vector from cube to nearest point on segment
	delta = vsub(ray_distance(a, b, org, 0), org);
	// get scalar distance from cube center to line
	dist = vlen(delta);
	// normalize vector to determine radius of cube in that direction
	delta = vscale(delta, 1.0f / dist);
	rad = radius * (fabs(delta.x) + fabs(delta.y) + fabs(delta.z));

	// intersecting ?
	if(dist <= rad)
		return 1;

	return 0;
}
//

//
int intersect_cube_triangle(vec3 org, float radius, vec3 v1, vec3 v2, vec3 v3)
{
	vec3 p, delta;
	float dist, rad;

//	org = vadd(org, (vec3){ 0.5, 0.5, 0.5 });

	// get normal of triangle
	delta = vnorm(vcross(vsub(v2, v1), vsub(v3, v1)));
	// get distance from cube org to triangle plane
	dist = vdot(delta, vsub(v1, org));
	rad = radius * (fabs(delta.x) + fabs(delta.y) + fabs(delta.z));

	// is plane too far from cube?
	if(fabs(dist) > rad)
		return 0;

	// get nearest point on triangle plane
	p = vadd(org, vscale(delta, dist));

	// is point inside of triangle edges?
	if(point_sameside(p, v1, v2, v3) && point_sameside(p, v2, v1, v3) && point_sameside(p, v3, v1, v2))
		return 1;

	// check distance to each edge of triangle
	if(intersect_ray_cube(v1, v2, org, radius) || intersect_ray_cube(v2, v3, org, radius) || intersect_ray_cube(v3, v1, org, radius))
		return 1;

	// nope..
	return 0;
}
//

//
float intersect_ray_triangle(vec3 a, vec3 b, vec3 v1, vec3 v2, vec3 v3)
{
	vec3 e1, e2, dir, x, y, z;
	float d, id, u, v;

	e1 = vsub(v2, v1);
	e2 = vsub(v3, v1);
	dir = vsub(b, a);

	x = vcross(dir, e2);
	d = vdot(e1, x);

	// avoid line being parallel with plane
	if(fabs(d) < EPSILON)
		return 0;

	// get inverse of determinant
	id = 1.0f / d;

	y = vsub(a, v1);		// line/plane verts dist
	z = vcross(y, e1);
	u = vdot(x, y) * id;

	if(u < 0.0 || u > 1.0)
		return 0;

	v = vdot(dir, z) * id;

	if(v < 0.0 || u + v > 1.0)
		return 0;

	// return line fraction
	return vdot(e2, z) * id;
}
//

/*
// returns zero on no intersection, or a fraction of the segment between 'ab' from 0->1
float intersect_ray_triangle(vec3 p1, vec3 p2, vec3 v1, vec3 v2, vec3 v3)
{
	vec3 dir, u, v, n, w, i;
	float uu, uv, vv, wu, wv, d;
	float x, y, z;

	// vector of segment
	dir = vsub(p2, p1);

	// get triangle normal
	u = vsub(v2, v1);
	v = vsub(v3, v1);
	n = vcross(u, v);

	// find intersection of line/plane
	w = vsub(v1, p1);
	x = vdot(n, w);
	y = vdot(n, dir);

	// are they parallel?
	if(fabs(y) < EPSILON)
		return 0;

	// get distance from p1 to intersection point on plane
	z = x / y;
	// i = ray/plane intersection point
	i = vadd(p1, vscale(dir, z));

	print("[ %f = %f / %f ] : ( %f, %f, %f )", z, x, y, i.x, i.y, i.z);

	// ensure intersection lies within p1-p2
	if(z <= 0 || z >= 1.0)
		return 0;

	// setup for barycentric check
	w = vsub(i, v1);
	wu = vdot(w, u);
	wv = vdot(w, v);

	// figure barycentric coordinates of 'i' in triangle
	uu = vdot(u, u);
	uv = vdot(u, v);
	vv = vdot(v, v);

	d = 1.0f / (uv * uv - uu * vv);
	x = (uv * wv - vv * wu) * d;

	if(x < 0 || x > 1)
	{
		print("x:%1.3f", x);
		return 0;
	}

	y = (uv * wu - uu * wv) * d;

	if(y < 0 || x + y > 1)
	{
		print("y:%1.3f", y);
		return 0;
	}

	// intersection!
	return z;
}
*/
