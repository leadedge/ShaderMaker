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
	// Time
	double m_Time;
	SYSTEMTIME m_SystemTimeStart;
	SYSTEMTIME m_SystemTimeLastFrame;
	double timeCounter;
	
	Vec3D position;
	Vec3D speeds;
	Vec3D scale;
	Vec3D rotation; 
	Vec3D translation;

	float m_dotcount;
	float m_dotsize;
	float m_beatdotsize;
	float m_beatdotrange;
	 float m_rounds;
	 float lastrounds;
	 float edginess;
	
	int m_initResources;

	GLuint m_heatTextureId;

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
	GLint m_inputTextureLocation;
	GLint m_heatTextureLocation;
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation; 
	GLint m_dotsizeLocation;
	 
	GLint m_positionsLocation;
	GLint m_edginessLocation;
	  
	
	GLint m_displayList;
};


#endif
