#include <windows.h>                          // Header File For Windows
#include <math.h>                         // Header File For Math Library Routines
#include <stdio.h>                            // Header File For Standard I/O Routines
#include <stdlib.h>                           // Header File For Standard Library
#include <gl\gl.h>                            // Header File For The OpenGL32 Library
#include <gl\glu.h>                           // Header File For The GLu32 Library

typedef struct point_3d {                       // Structure For A 3-Dimensional Point ( NEW )
    double x, y, z;
} POINT_3D;
 
typedef struct bpatch {                         // Structure For A 3rd Degree Bezier Patch ( NEW )
    POINT_3D    anchors[4][4];                  // 4x4 Grid Of Anchor Points
    GLuint      dlBPatch;                   // Display List For Bezier Patch
    GLuint      texture;                    // Texture For The Patch
} BEZIER_PATCH;
 
HDC         hDC=NULL;                   // Private GDI Device Context
HGLRC           hRC=NULL;                   // Permanent Rendering Context
HWND            hWnd=NULL;                  // Holds Our Window Handle
HINSTANCE       hInstance;                  // Holds The Instance Of The Application
 
DEVMODE         DMsaved;                    // Saves The Previous Screen Settings ( NEW )
 
bool            keys[256];                  // Array Used For The Keyboard Routine
bool            active=TRUE;                    // Window Active Flag Set To TRUE By Default
bool            fullscreen=TRUE;                // Fullscreen Flag Set To Fullscreen Mode By Default
 
GLfloat         rotz = 0.0f;                    // Rotation About The Z Axis
BEZIER_PATCH        mybezier;                   // The Bezier Patch We're Going To Use ( NEW )
BOOL            showCPoints=TRUE;               // Toggles Displaying The Control Point Grid ( NEW )
int         divs = 7;                   // Number Of Intrapolations (Controls Poly Resolution) ( NEW )
 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);           // Declaration For WndProc

// Adds 2 Points. Don't Just Use '+' ;)
POINT_3D pointAdd(POINT_3D p, POINT_3D q) {
    p.x += q.x;     p.y += q.y;     p.z += q.z;
    return p;
}
 
// Multiplies A Point And A Constant. Don't Just Use '*'
POINT_3D pointTimes(double c, POINT_3D p) {
    p.x *= c;   p.y *= c;   p.z *= c;
    return p;
}
 
// Function For Quick Point Creation
POINT_3D makePoint(double a, double b, double c) {
    POINT_3D p;
    p.x = a;    p.y = b;    p.z = c;
    return p;
}

// Calculates 3rd Degree Polynomial Based On Array Of 4 Points
// And A Single Variable (u) Which Is Generally Between 0 And 1
POINT_3D Bernstein(float u, POINT_3D *p) {
    POINT_3D    a, b, c, d, r;
 
    a = pointTimes(pow(u,3), p[0]);
    b = pointTimes(3*pow(u,2)*(1-u), p[1]);
    c = pointTimes(3*u*pow((1-u),2), p[2]);
    d = pointTimes(pow((1-u),3), p[3]);
 
    r = pointAdd(pointAdd(a, b), pointAdd(c, d));
 
    return r;
}

// Generates A Display List Based On The Data In The Patch
// And The Number Of Divisions
GLuint genBezier(BEZIER_PATCH patch, int divs,FFGLExtensions m_extensions) {
  	int			u = 0, v;
	float		py, px, pyold; 
	GLuint		drawlist = glGenLists(1);		// make the display list
	POINT_3D	temp[4];
	POINT_3D	*last = (POINT_3D*)malloc(sizeof(POINT_3D)*(divs+1));
												// array of points to mark the first line of polys

	if (patch.dlBPatch != NULL)					// get rid of any old display lists
		glDeleteLists(patch.dlBPatch, 1);

	temp[0] = patch.anchors[0][3];				// the first derived curve (along x axis)
	temp[1] = patch.anchors[1][3];
	temp[2] = patch.anchors[2][3];
	temp[3] = patch.anchors[3][3];

	for (v=0;v<=divs;v++) {						// create the first line of points
		px = ((float)v)/((float)divs);			// percent along y axis
	// use the 4 points from the derives curve to calculate the points along that curve
		last[v] = Bernstein(px, temp);
	}

	glNewList(drawlist, GL_COMPILE);				// Start a new display list
	glBindTexture(GL_TEXTURE_2D, patch.texture);	// Bind the texture

	for (u=1;u<=divs;u++) {
		py	  = ((float)u)/((float)divs);			// Percent along Y axis
		pyold = ((float)u-1.0f)/((float)divs);		// Percent along old Y axis

		temp[0] = Bernstein(py, patch.anchors[0]);	// Calculate new bezier points
		temp[1] = Bernstein(py, patch.anchors[1]);
		temp[2] = Bernstein(py, patch.anchors[2]);
		temp[3] = Bernstein(py, patch.anchors[3]);

		glBegin(GL_TRIANGLE_STRIP);					// Begin a new triangle strip

		for (v=0;v<=divs;v++) {
			px = ((float)v)/((float)divs);			// Percent along the X axis

		//	glTexCoord2f(pyold, px);				// Apply the old texture coords
		m_extensions.glMultiTexCoord2f(GL_TEXTURE1,pyold,px);

			glVertex3d(last[v].x, last[v].y, last[v].z);	// Old Point

			last[v] = Bernstein(px, temp);			// Generate new point
	 	glTexCoord2f(py, px);					// Apply the new texture coords
		
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1,py,px);
			glVertex3d(last[v].x, last[v].y, last[v].z);	// New Point
		}

		glEnd();									// END the triangle srip
	}
	
	glEndList();								// END the list

	free(last);									// Free the old vertices array
	return drawlist;							// Return the display list
}


void initBezier(void) {
    mybezier.anchors[0][0] = makePoint(-1.0,   -0.75,  0.00); // Set The Bezier Vertices
    mybezier.anchors[0][1] = makePoint(-0.33,   -0.75,   0.00);
    mybezier.anchors[0][2] = makePoint( 0.33,   -0.75,   0.00);
    mybezier.anchors[0][3] = makePoint( 1.0,   -0.75,  0.00);
    mybezier.anchors[1][0] = makePoint(-1.0,   -0.25,  0.0);
    mybezier.anchors[1][1] = makePoint(-0.33,   -0.25,   0.00);
    mybezier.anchors[1][2] = makePoint( 0.33,   -0.25,   0.00);
    mybezier.anchors[1][3] = makePoint( 1.0,   -0.25,  0.0);
    mybezier.anchors[2][0] = makePoint(-0.75,    0.225,   0.00);
    mybezier.anchors[2][1] = makePoint(-0.25,    0.25,  0.00);
    mybezier.anchors[2][2] = makePoint( 0.25,   0.25,  0.00);
    mybezier.anchors[2][3] = makePoint( 0.75,    0.25,   0.00);
    mybezier.anchors[3][0] = makePoint(-0.75,    0.75,  0.00);
    mybezier.anchors[3][1] = makePoint(-0.25,    0.75,  0.00);
    mybezier.anchors[3][2] = makePoint( 0.25,    0.75,  0.00);
    mybezier.anchors[3][3] = makePoint( 0.75,    0.75,  0.00);
    mybezier.dlBPatch = NULL;                   // Go Ahead And Initialize This To NULL
}

int DrawGLScene(GLvoid) {                       // Here's Where We Do All The Drawing
	  int i, j;

  /* glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);                    // Enables Depth Testing
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear Screen And Depth Buffer
    glLoadIdentity();                       // Reset The Current Modelview Matrix
    glTranslatef(0.0f,0.0f,-4.0f);                  // Move Left 1.5 Units And Into The Screen 6.0
    glRotatef(-75.0f,1.0f,0.0f,0.0f);
    glRotatef(rotz,0.0f,0.0f,1.0f);                 // Rotate The Triangle On The Z-Axis
         */
    glCallList(mybezier.dlBPatch);                  // Call The Bezier's Display List
                                    // This Need Only Be Updated When The Patch Changes
 
    if (showCPoints) {                      // If Drawing The Grid Is Toggled On
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f,1.0f,1.0f);
		glLineWidth(10);

        for(i=0;i<4;i++) {                   // Draw The Horizontal Lines
            glBegin(GL_LINE_STRIP);
            for(j=0;j<4;j++)
                glVertex3d(mybezier.anchors[i][j].x, mybezier.anchors[i][j].y, mybezier.anchors[i][j].z);
            glEnd();
        }
        for(i=0;i<4;i++) {                   // Draw The Vertical Lines
            glBegin(GL_LINE_STRIP);
            for(j=0;j<4;j++)
                glVertex3d(mybezier.anchors[j][i].x, mybezier.anchors[j][i].y, mybezier.anchors[j][i].z);
            glEnd();
        }
        glColor3f(1.0f,1.0f,1.0f);
        glEnable(GL_TEXTURE_2D);
    }
 
    return TRUE;                            // Keep Going
}