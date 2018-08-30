/* Draw a torus */
#include "ShaderMaker.h"
	

float lerp(float a,float b, float t){
	
	return a + (b - a)*t;
} 
Vector torusValue(float angleZ, float angleInner,float torusRadius,float torusWidth) {

	Vector result;
	result.x = (torusWidth + torusRadius*cos(angleZ))*cos(angleInner);
	result.y = (torusWidth + torusRadius*cos(angleZ))*sin(angleInner);
	result.z = torusRadius * sin(angleZ);

	return result;
}
void torus(int numc, int numt,float radius, float width) {

	int i, j, k;
	double s, t, x, y, z, twopi;

	twopi = 2 * PI;
	float stepx = numc / twopi;
	float stepy = numc / twopi;
	for (i = 0; i < numc; i++) {
		for (j = 0; j <= numt; j++) {

			Vector pos = torusValue(float(j)*stepy, float(i)*stepx,radius,width);

			glVertex3f(pos.x, pos.y, pos.z);
			//	printf("Vertex is %f %f %f\n", x, y, z);

		}
	}

}
void torus(int numc, int numt)
{
	return torus(numc, numt, 0.2, 1.0);
}