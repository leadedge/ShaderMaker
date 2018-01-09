#include <FFGL.h>
#include <FFGLLib.h>
#include "FFGLTime.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo (
	FFGLTime::CreateInstance,	// Create method
	"GTME",	// Plugin unique ID
	"FFGLTime", // Plugin name
	1,									// API major version number
	000,								  // API minor version number
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"Sample FFGL Time plugin",	// Plugin description
	"by Trey Harrison - www.harrisondigitalmedia.com" // About
);


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLTime::FFGLTime()
: CFreeFrameGLPlugin()
{
	// Input properties
	SetMinInputs(1);
	SetMaxInputs(1);
  SetTimeSupported(true);
}

FFGLTime::~FFGLTime()
{ 
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD FFGLTime::SetTime(double time)
{
  m_curTime = time;
  return FF_SUCCESS;
}

DWORD FFGLTime::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
  //set color to white
  glColor4f(1.f,1.f,1.f,1.f);

  //start out by just filling our output with the input texture
  if (pGL->numInputTextures<1 || pGL->inputTextures[0]==NULL)
  {
    //no input? just clear
    glClearColor(0.f,0.f,0.f,0.f);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  else
  {
    FFGLTextureStruct &Texture = *(pGL->inputTextures[0]);

    //bind the texture handle
    glBindTexture(GL_TEXTURE_2D, Texture.Handle);

    //enable texturemapping
    glEnable(GL_TEXTURE_2D);
    
    //get the max s,t that correspond to the 
    //width,height of the used portion of the allocated texture space
    FFGLTexCoords maxCoords = GetMaxGLTexCoords(Texture);

    glBegin(GL_QUADS);
    //lower left
    glTexCoord2d(0.0, 0.0);
    glVertex2f(-1,-1);
    //upper left
    glTexCoord2d(0.0, maxCoords.t);
    glVertex2f(-1,1);
    //upper right
    glTexCoord2d(maxCoords.s, maxCoords.t);
    glVertex2f(1,1);
    //lower right
    glTexCoord2d(maxCoords.s, 0.0);
    glVertex2f(1,-1);
    glEnd();

    //unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    //disable texturemapping
    glDisable(GL_TEXTURE_2D);
  }

  //now overlay the current time

  //scale the matrix so that our characters actually fit nicely in the center of
  //the output
  glScalef(0.25,0.25,1.0);
  glTranslatef(-2.5,-1.0,0.0);

  //make our lines and points bigger
  glLineWidth(4.0);
  glPointSize(4.0);

  //generate a string containing the time, for example '1.49'

  char timeString[32];
  //be very careful with sprintf, it ignores the amount of memory
  //you have available to store the string output and is often a source
  //of buffer overflow exploits
  sprintf(timeString, "%.2f", m_curTime);
  
  //draw the appropriate graphics for each character of the string
  char *c = timeString;
  while (*c)
  {
    switch (*c)
    {
      case '0': Draw0(); break;
      case '1': Draw1(); break;
      case '2': Draw2(); break;
      case '3': Draw3(); break;
      case '4': Draw4(); break;
      case '5': Draw5(); break;
      case '6': Draw6(); break;
      case '7': Draw7(); break;
      case '8': Draw8(); break;
      case '9': Draw9(); break;
      case '.': DrawDecimal(); break;
    }

    //use the matrix to draw the next character to the right
    glTranslatef(1.25, 0.f, 0.f);
    c++;
  }

  //restore default line width
  glLineWidth(1.0);

  //restore default point size
  glPointSize(1.0);

  //restore the default matrix value
  //(default matrix value in OpenGL is the identity matrix)
  glLoadIdentity();

  return FF_SUCCESS;
}

void FFGLTime::Draw0()
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(0,0);
  glVertex2f(0,2);
  glVertex2f(1,2);
  glVertex2f(1,0);
  glEnd();
}

void FFGLTime::Draw1()
{
  glBegin(GL_LINE_STRIP);
  glVertex2f(1,2);
  glVertex2f(1,0);
  glEnd();
}

void FFGLTime::Draw2()
{
  glBegin(GL_LINE_STRIP);
  glVertex2f(0,2);
  glVertex2f(1,2);
  glVertex2f(1,1);
  glVertex2f(0,1);
  glVertex2f(0,0);
  glVertex2f(1,0);
  glEnd();
}

void FFGLTime::Draw3()
{
  glBegin(GL_LINE_STRIP);
  glVertex2f(0,2);
  glVertex2f(1,2);
  glVertex2f(1,1);
  glVertex2f(0,1);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(1,1);
  glVertex2f(1,0);
  glVertex2f(0,0);
  glEnd();
}

void FFGLTime::Draw4()
{
  glBegin(GL_LINE_STRIP);
  glVertex2f(0,2);
  glVertex2f(0,1);
  glVertex2f(1,1);
  glVertex2f(1,2);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(1,1);
  glVertex2f(1,0);
  glEnd();
}

void FFGLTime::Draw5()
{
  glBegin(GL_LINE_STRIP);
  glVertex2f(1,2);
  glVertex2f(0,2);
  glVertex2f(0,1);
  glVertex2f(1,1);
  glVertex2f(1,0);
  glVertex2f(0,0);
  glEnd();
}

void FFGLTime::Draw6()
{
  glBegin(GL_LINE_STRIP);
  glVertex2f(1,2);
  glVertex2f(0,2);
  glVertex2f(0,1);
  glVertex2f(1,1);
  glVertex2f(1,0);
  glVertex2f(0,0);
  glVertex2f(0,1);
  glEnd();
}


void FFGLTime::Draw7()
{
  glBegin(GL_LINE_STRIP);
  glVertex2f(0,2);
  glVertex2f(1,2);
  glVertex2f(1,0);
  glEnd();
}

void FFGLTime::Draw8()
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(0,2);
  glVertex2f(1,2);
  glVertex2f(1,1);
  glVertex2f(0,1);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(1,1);
  glVertex2f(1,0);
  glVertex2f(0,0);
  glVertex2f(0,1);
  glEnd();
}

void FFGLTime::Draw9()
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(0,2);
  glVertex2f(1,2);
  glVertex2f(1,1);
  glVertex2f(0,1);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(1,1);
  glVertex2f(1,0);
  glVertex2f(0,0);
  glEnd();
}

void FFGLTime::DrawDecimal()
{
  glBegin(GL_POINTS);
  glVertex2f(0.5, 0);
  glEnd();
}

