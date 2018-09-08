#ifndef FFGLHeat_H
#define FFGLHeat_H

#include <FFGLShader.h>
#include "../FFGLPluginSDK.h"


struct Vec3D{

float x;
float y;
float z;
};
class FFGLHeat :
public CFreeFrameGLPlugin
{
public:
	FFGLHeat();
  virtual ~FFGLHeat() {}

	///////////////////////////////////////////////////
	// FreeFrameGL plugin methods
	///////////////////////////////////////////////////
	
	DWORD	SetParameter(const SetParameterStruct* pParam);		
	DWORD	GetParameter(DWORD dwIndex);					
	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);
  DWORD InitGL(const FFGLViewportStruct *vp);
  DWORD DeInitGL();
  	DWORD SetTime(double time);
	
	

	
void renderQuad(float texMaxX,float texMaxY);
void updateRenderTargets(int newWidth,int newHeight);
double GetTimeInSecondsSinceStart();
void createDisplayList();
	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
  	*ppOutInstance = new FFGLHeat();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }
protected:	
	static const int torusCount=125;
	// Time
	double m_Time;
	SYSTEMTIME m_SystemTimeStart;
	SYSTEMTIME m_SystemTimeLastFrame;
	double timeCounter;
	float decay;
	bool useFollow;
	
	Vec3D decayRot;
	Vec3D decayShift;
	Vec3D decayRotCurrent;
	Vec3D decayShiftCurrent;

	Vec3D times1;
	Vec3D times2;
	Vec3D position;
	Vec3D swayRot;
	Vec3D swayShift;
 
	Vec3D speeds;
	Vec3D speeds2;
	Vec3D scale;
	Vec3D rotation; 
	Vec3D translation;

	int lastWidth;
	int lastHeight;
	GLuint renderedTexture1; 
	GLuint renderedTexture1Depth; 
	
	GLuint m_FramebufferId;


	
	Vec3D rotations[torusCount];
	Vec3D positions[torusCount];
	 
	 float m_rounds;
	 float lastrounds; 
	int m_initResources;
	 

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
    FFGLShader m_shaderFinal;

	GLint m_inputTextureLocation; 
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation;  
	 
	GLint m_positionsLocation; 
	  GLint m_useTextureLocation;
	
	GLint m_displayList[1000];
};


#endif
