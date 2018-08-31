/* Draw a torus */
#include "ShaderMaker.h"

float lerp(float a, float b, float t) {

	return a + (b - a)*t;
}
float lerpifShorter(float a, float b, float t) {

	float distNormal = abs(a - b);
	float distOtherDirection = abs((a - (b + 1.0)));
	float distOtherDirection2 = abs((a - (b - 1.0)));

	if (distNormal <= distOtherDirection&&distNormal <= distOtherDirection2) {


		return a + (b - a)*t;

	}
	else if (distOtherDirection <= distNormal &&distOtherDirection <= distOtherDirection2) {

		return a + ((b+1.0) - a)*t-1.;

	}
	else if (distOtherDirection2 <= distNormal &&distOtherDirection2 <= distOtherDirection) {

		return a + ((b-1.0) - a)*t+1.0;
	}

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
	float stepy = numt / twopi;
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

void DrawTorus(float radius, float width, float r, float g, float b) {
	int numc = 16, numt = 36;

	double TWOPI = 2 * PI;
	for (int i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= numt; j++) {
			for (int k = 1; k >= 0; k--) {

				double s = (i + k) % numc + 0.5;
				double t = j % numt;

				double x = (radius + width * cos(s * TWOPI / numc)) * cos(t * TWOPI / numt);
				double y = (radius + width * cos(s * TWOPI / numc)) * sin(t * TWOPI / numt);
				double z = width * sin(s * TWOPI / numc);

				// calculate the normal        
				double nx = (radius + 0 * cos(s * TWOPI / numc)) * cos(t * TWOPI / numt);
				double ny = (radius + 0 * cos(s * TWOPI / numc)) * sin(t * TWOPI / numt);
				double nz = 0 * sin(s * TWOPI / numc);

				double dx = x - nx;
				double dy = y - ny;
				double dz = z - nz;

				double l = sqrt(dx*dx + dy*dy + dz*dz);

				glNormal3d(dx / l, dy / l, dz / l);
				glColor3f(r, g, b);
				glVertex3d(x, z, y);



			}
		}
		glEnd();
	}
}