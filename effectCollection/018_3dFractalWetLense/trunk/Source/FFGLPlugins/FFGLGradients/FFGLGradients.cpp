#include <FFGL.h>
#include <FFGLLib.h>
#include "FFGLGradients.h"
#include <math.h>

#define FFPARAM_Hue1  (0)
#define FFPARAM_Hue2  (1)
#define FFPARAM_Saturation  (2)
#define FFPARAM_Brightness  (3)


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo (
	FFGLGradients::CreateInstance,	// Create method
	"GRAD",								// Plugin unique ID
	"Gradients",						// Plugin name
	1,									// API major version number
	000,								// API minor version number
	1,									// Plugin major version number
	000,								// Plugin minor version number
	FF_SOURCE,							// Plugin type
	"Sample FFGL Gradients plugin",		// Plugin description
	"by Edwin de Koning - www.resolume.com" // About
);


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLGradients::FFGLGradients()
:CFreeFrameGLPlugin()
{

	// Input properties
	SetMinInputs(0);
	SetMaxInputs(0);

	// Parameters
	SetParamInfo(FFPARAM_Hue1, "Hue 1", FF_TYPE_STANDARD, 0.0f);
	m_Hue1 = 0.0f;

	SetParamInfo(FFPARAM_Hue2, "Hue 2", FF_TYPE_STANDARD, 0.5f);
	m_Hue2 = 0.5f;

	SetParamInfo(FFPARAM_Saturation, "Saturation", FF_TYPE_STANDARD, 1.0f);
	m_Saturation = 1.0f;

	SetParamInfo(FFPARAM_Brightness, "Brightness", FF_TYPE_STANDARD, 1.0f);
	m_Brightness = 1.0f;



}

DWORD FFGLGradients::InitGL(const FFGLViewportStruct *vp)
{


	return FF_SUCCESS;
}

DWORD FFGLGradients::DeInitGL()
{

  return FF_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////



DWORD FFGLGradients::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{

	double rgb1[3];

	//we need to make sure the hue doesn't reach 1.0f, otherwise the result will be pink and not red how it should be
	double hue1 = (m_Hue1 > 0.99) ? 0.99 : m_Hue1;
	HSVtoRGB( hue1, m_Saturation, m_Brightness, &rgb1[0], &rgb1[1], &rgb1[2]);

	double rgb2[3];
	double hue2 = (m_Hue2 > 0.99) ? 0.99 : m_Hue2;
	HSVtoRGB( hue2, m_Saturation, m_Brightness, &rgb2[0], &rgb2[1], &rgb2[2]);


	glShadeModel(GL_SMOOTH);
	glBegin(GL_POLYGON);
		glColor3f( rgb1[0], rgb1[1], rgb1[2] );
		glVertex2f(-1.0, -1.0);	// Bottom Left Of The Texture and Quad

		glColor3f( rgb2[0], rgb2[1], rgb2[2] );
		glVertex2f( 1.0, -1.0);	// Bottom Right Of The Texture and Quad

		glColor3f( rgb2[0], rgb2[1], rgb2[2] );
		glVertex2f( 1.0,  1.0);	// Top Right Of The Texture and Quad

		glColor3f( rgb1[0], rgb1[1], rgb1[2] );
		glVertex2f(-1.0,  1.0);	// Top Left Of The Texture and Quad
	glEnd();


	return FF_SUCCESS;
}

DWORD FFGLGradients::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

	switch (dwIndex) {

		case FFPARAM_Hue1:
			*((float *)(unsigned)&dwRet) = m_Hue1;
				return dwRet;
		case FFPARAM_Hue2:
			*((float *)(unsigned)&dwRet) = m_Hue2;
				return dwRet;
		case FFPARAM_Saturation:
			*((float *)(unsigned)&dwRet) = m_Saturation;
				return dwRet;
		case FFPARAM_Brightness:
			*((float *)(unsigned)&dwRet) = m_Brightness;
				return dwRet;
		default:
			return FF_FAIL;
	}
}

DWORD FFGLGradients::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam != NULL) {

		switch (pParam->ParameterNumber) {

			case FFPARAM_Hue1:
				m_Hue1 = *((float *)(unsigned)&(pParam->NewParameterValue));
				break;
			case FFPARAM_Hue2:
				m_Hue2 = *((float *)(unsigned)&(pParam->NewParameterValue));
				break;
			case FFPARAM_Saturation:
				m_Saturation = *((float *)(unsigned)&(pParam->NewParameterValue));
				break;
			case FFPARAM_Brightness:
				m_Brightness = *((float *)(unsigned)&(pParam->NewParameterValue));
				break;
			default:
				return FF_FAIL;
		}

		return FF_SUCCESS;

	}

	return FF_FAIL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Utils
////////////////////////////////////////////////////////////////////////////////////////////////////
void HSVtoRGB(double h, double s, double v, double* r, double* g, double* b)
{

  if ( s == 0 )

  {

     *r = v;

     *g = v;

     *b = v;

  } else {

     double var_h = h * 6;

     double var_i = floor( var_h );

	 double var_1 = v * ( 1 - s );

	 double var_2 = v * ( 1 - s * ( var_h - var_i ) );

	 double var_3 = v * ( 1 - s * ( 1 - ( var_h - var_i ) ) );


     if      ( var_i == 0 ) { *r = v     ; *g = var_3 ; *b = var_1; }

     else if ( var_i == 1 ) { *r = var_2 ; *g = v     ; *b = var_1; }

     else if ( var_i == 2 ) { *r = var_1 ; *g = v     ; *b = var_3; }

     else if ( var_i == 3 ) { *r = var_1 ; *g = var_2 ; *b = v;     }

     else if ( var_i == 4 ) { *r = var_3 ; *g = var_1 ; *b = v;     }

     else                   { *r = v     ; *g = var_1 ; *b = var_2; }



  }

}


