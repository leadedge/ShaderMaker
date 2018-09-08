#ifndef FFGLHeat_H
#define FFGLHeat_H

#include <FFGLShader.h>
#include "../FFGLPluginSDK.h"
 
struct Vector2D{
	float x;
	float y;
};
struct Vector3D{
	float x;
	float y;
	float z;
};

struct Anim{
	float count;
		float speed;
};

class FFGLSineWave2D :


public CFreeFrameGLPlugin
{
public:
	FFGLSineWave2D();
  virtual ~FFGLSineWave2D() {}

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
	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
  	*ppOutInstance = new FFGLSineWave2D();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }
double systemTimeToDouble(SYSTEMTIME *time);
protected:	
	// Time
	double m_Time;
	SYSTEMTIME m_SystemTimeStart;
	SYSTEMTIME m_SystemTimeLastFrame;
	double timeCounter;   
	 
	 
	double param2x;
	double param2y;
	double param2z;
	double param2w;

	Anim positionAnimX;
	Anim positionAnimY;

	int m_initResources;
	Vector3D hsvScale;
	Vector3D cameraRotation;
	Vector2D center; 


	float targetDistance;
	Vector3D yawpitchroll;
	Vector3D camerapos;


	GLuint m_heatTextureId;

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
	GLint m_inputTextureLocation; 
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation;  
	  

	 
	GLint m_resolutionLocation; 
	GLint m_hsvScaleLocation;  

	
	GLint m_distanceLocation;  
	GLint m_yawpitchLocation;  
	GLint m_camposLocation;  
	GLint m_param2Location;  

	 
};


#endif
