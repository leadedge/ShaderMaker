
#include <FFGL.h>
#include <FFGLLib.h>
#include "math.h"
#include "FFGLHeat.h"
#include "glu.h"
#include "glext.h"
#include "color_hsv_rgb.h"
#include<string>
#include <iostream>
#define STRINGIFY(A) #A


#define PI 3.14159265359
  
#define FFPARAM_SPEED_ROT_X     (0)
#define FFPARAM_SPEED_ROT_Y     (1)
#define FFPARAM_SPEED_ROT_Z  (2) 

#define FFPARAM_ROTX  (3) 
#define FFPARAM_ROTY	  (4)
#define FFPARAM_ROTZ	  (5) 

#define FFPARAM_ROT_SWAY_X  (6) 
#define FFPARAM_ROT_SWAY_Y	  (7)
#define FFPARAM_ROT_SWAY_Z	  (8) 

#define FFPARAM_SPEED_SHIFT_X     (9)
#define FFPARAM_SPEED_SHIFT_Y     (10)
#define FFPARAM_SPEED_SHIFT_Z  (11) 


#define FFPARAM_TRANSLATEX  (12) 
#define FFPARAM_TRANSLATEY	  (13)
#define FFPARAM_TRANSLATEZ	  (14) 

#define FFPARAM_TRANSLATE_SWAY_X  (15) 
#define FFPARAM_TRANSLATE_SWAY_Y	  (16)
#define FFPARAM_TRANSLATE_SWAY_Z	  (17) 
  
#define FFPARAM_DECAY  (18) 

#define FFPARAM_DECAY_ROT_X  (19) 
#define FFPARAM_DECAY_ROT_Y  (20) 
#define FFPARAM_DECAY_ROT_Z  (21) 


#define FFPARAM_DECAY_SHIFT_X  (22) 
#define FFPARAM_DECAY_SHIFT_Y  (23) 
#define FFPARAM_DECAY_SHIFT_Z  (24) 

#define FFPARAM_FOLLOW_OR_DECAY     (25)

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLHeat::CreateInstance,	// Create method
	"S065",								// Plugin unique ID											
	"sOm-Tori",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_SOURCE,						// Plugin type
	"sOm-tori",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis aug2014 - Noise Points. " // About
); 

char *vertexShaderCode =STRINGIFY(
uniform sampler2D inputTexture;
uniform vec2 maxCoords;
uniform vec4 sineFactors;
uniform vec4 sineFactors2;
uniform float heatAmount;
uniform float time;
uniform float BPM; 
uniform float dotsize;  
uniform vec3 positions; 
varying vec3 N;
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
float decay(float start,float decayRate, float t)
{
return start*pow(decayRate,t);
}
void main()
{ 
	 N = normalize(gl_NormalMatrix * gl_Normal);
gl_Position =gl_ModelViewProjectionMatrix*vec4(gl_Vertex.xyz,1);
  gl_FrontColor =gl_Color;
    gl_PointSize =2.0;
		  gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;
}
)
;
/*
sin(gl_Vertex.x*sineFactors.y+sineFactors.x+gl_Vertex.y*sineFactors2.y)*amplitudes.x+"
"sin(gl_Vertex.x*sineFactors.w+sineFactors.z+gl_Vertex.y*sineFactors2.w)*amplitudes.y+"
"					cos(gl_Vertex.y*sineFactors2.y+sineFactors2.z+gl_Vertex.x*sineFactors.y)*amplitudes.z+	cos(gl_Vertex.y*sineFactors2.w+sineFactors2.z+gl_Vertex.x*sineFactors.w)*amplitudes.z,0"
"
*/
	
char *fragmentShaderCode =STRINGIFY(
	varying vec3 N;
varying vec2 varyingTextureCoordinate;
float PI=3.14159265359;
uniform float aspectRatio;
uniform sampler2D inputTexture; 
uniform vec2 maxCoords;
uniform vec4 sineFactors;
uniform vec4 sineFactors2;
uniform float heatAmount;
uniform float time;
uniform float dotsize;
uniform float edginess; 
uniform float useTexture;

vec3 lightDir=vec3(1,1,1);

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
vec3 createColor(float time){
	vec3 currentColor=hsv2rgb(vec3(sin(time),1,1));
	return currentColor;
}
void main() 
{
   vec2 texCoord =gl_TexCoord[1].st; 
	if(useTexture<0.5){
   vec3 col_out = gl_Color.xyz; 
   // lighting
   float light=clamp(dot(N,lightDir),0,1);
   float ambient=.50;
//   gl_FragColor = vec4(N.x,N.y,N.z,1);
   gl_FragColor = vec4(col_out*ambient+col_out*light,1);
	}else{
	 vec4 c = texture2D(inputTexture, texCoord);
  	gl_FragColor = c;
//	gl_FragColor.xy=texCoord;
	}

}
); 




////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLHeat::FFGLHeat()
:CFreeFrameGLPlugin(),
 m_initResources(1), 
 m_inputTextureLocation(-1), 
 m_maxCoordsLocation(-1)  
{
	// Input properties
	SetMinInputs(1);
	SetMaxInputs(1);
	SetTimeSupported(true);
	
	// Parameters
	// Param swizzle, somehow BPM is not accepted ... right now .. ;) 
	SetParamInfo(FFPARAM_SPEED_ROT_X, "SpeedRotX", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_SPEED_ROT_Y, "SpeedRotY", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_SPEED_ROT_Z, "SpeedRotZ", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_SPEED_SHIFT_X, "SpeedShiftX", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_SPEED_SHIFT_Y, "SpeedShiftY", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_SPEED_SHIFT_Z, "SpeedShiftZ", FF_TYPE_STANDARD, 0.5f);  
	SetParamInfo(FFPARAM_ROTX, "RotationX", FF_TYPE_STANDARD, 0.52f); 
	SetParamInfo(FFPARAM_ROTY, "RotationY", FF_TYPE_STANDARD, 0.52f); 
	SetParamInfo(FFPARAM_ROTZ, "RotationZ", FF_TYPE_STANDARD, 0.52f);  

	SetParamInfo(FFPARAM_ROT_SWAY_X, "RotationSwayX", FF_TYPE_STANDARD, 0.52f); 
	SetParamInfo(FFPARAM_ROT_SWAY_Y, "RotationSwayY", FF_TYPE_STANDARD, 0.52f); 
	SetParamInfo(FFPARAM_ROT_SWAY_Z, "RotationSwayZ", FF_TYPE_STANDARD, 0.52f);  

	SetParamInfo(FFPARAM_TRANSLATEX, "TranslateX", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_TRANSLATEY, "TranslateY", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_TRANSLATEZ, "TranslateZ", FF_TYPE_STANDARD, 0.5f); 
	
	SetParamInfo(FFPARAM_TRANSLATE_SWAY_X, "TranslateSwayX", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_TRANSLATE_SWAY_Y, "TranslateSwayY", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_TRANSLATE_SWAY_Z, "TranslateSwayZ", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_DECAY, "Decay", FF_TYPE_STANDARD, 0.5f); 

	
	SetParamInfo(FFPARAM_DECAY_ROT_X, "DecayRotX", FF_TYPE_STANDARD, 0.01f); 
	SetParamInfo(FFPARAM_DECAY_ROT_Y, "DecayRotY", FF_TYPE_STANDARD, 0.01f); 
	SetParamInfo(FFPARAM_DECAY_ROT_Z, "DecayRotZ", FF_TYPE_STANDARD, 0.01f); 

	SetParamInfo(FFPARAM_DECAY_SHIFT_X, "DecayShiftX", FF_TYPE_STANDARD, 0.01f); 
	SetParamInfo(FFPARAM_DECAY_SHIFT_Y, "DecayShiftY", FF_TYPE_STANDARD, 0.01f); 
	SetParamInfo(FFPARAM_DECAY_SHIFT_Z, "DecayShiftZ", FF_TYPE_STANDARD, 0.01f); 
	SetParamInfo(FFPARAM_FOLLOW_OR_DECAY, "Decay/Follow", FF_TYPE_BOOLEAN, 0.00f); 


	//SetParamInfo(FFPARAM_BPM, "BPM(*240)", FF_TYPE_STANDARD, 0.5f);  

	timeCounter=0;
	decayRot.x=0.1;
decayRot.y=0.2;
decayRot.z=0.3;

	position.x=0;
	position.y=0;
	position.z=0; 
	m_rounds=0.5;
	lastrounds=0.5;
	
	decayRot.x=0.1;
	decayRot.y=0.1;
	decayRot.z=0.1;
	
	decayRotCurrent.x=0.1;
	decayRotCurrent.y=0.1;
	decayRotCurrent.z=0.1;
	
	decayShift.x=0.1;
	decayShift.y=0.1;
	decayShift.z=0.1;
	
	decayShiftCurrent.x=0.1;
	decayShiftCurrent.y=0.1;
	decayShiftCurrent.z=0.1;
	

	for(int i =0;i<torusCount;i++){
	
	rotations[i].x=0;
	rotations[i].y=0;
	rotations[i].z=0;

	positions[i].x=0;
	positions[i].y=0;
	positions[i].z=0;

	}
	
	times1.x=0;
	times1.y=0;
	times1.z=0;
	times2.x=0;
	times2.y=0;
	times2.z=0;

	swayRot.x=45;
	swayRot.y=45;
	swayRot.z=10;
	 
	GetSystemTime(&m_SystemTimeLastFrame); 
	// Initialise System Time Based Time counting	 
    GetSystemTime(&m_SystemTimeStart);

	m_Time=0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD FFGLHeat::InitGL(const FFGLViewportStruct *vp)
{
  //initialize gl extensions and
  //make sure required features are supported
  m_extensions.Initialize();
  if (m_extensions.multitexture==0 || m_extensions.ARB_shader_objects==0)
    return FF_FAIL; 
  //initialize gl shader
  m_shader.SetExtensions(&m_extensions);
  m_shader.Compile(vertexShaderCode,fragmentShaderCode);
 
  //activate our shader
  m_shader.BindShader();
    
  //to assign values to parameters in the shader, we have to lookup
  //the "location" of each value.. then call one of the glUniform* methods
  //to assign a value
  m_inputTextureLocation = m_shader.FindUniform("inputTexture"); 
  m_maxCoordsLocation = m_shader.FindUniform("maxCoords"); 
  m_timeLocation = m_shader.FindUniform("time"); 
  m_aspectLocation= m_shader.FindUniform("aspectRatio");  
  
  m_positionsLocation= m_shader.FindUniform("positions"); 
  m_useTextureLocation= m_shader.FindUniform("useTexture");

  //the 0 means that the 'heatTexture' in
  //the shader will use the texture bound to GL texture unit 0 
    
  //the 1 means that the 'inputTexture' in
  //the shader will use the texture bound to GL texture unit 1
  m_extensions.glUniform1iARB(m_inputTextureLocation, 1);
    
  m_shader.UnbindShader();

  //create/upload the heat texture on texture unit 0
  //(which should already be the active unit)
    
  createDisplayList();

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     
  
	m_extensions.glGenFramebuffersEXT(1, &m_FramebufferId);
	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER, m_FramebufferId);
	glGenTextures(1, &renderedTexture1);
	glGenTextures(1, &renderedTexture1Depth);


  return FF_SUCCESS;
}
 

float myRandom(float min, float max){
 
return min+((float)rand()/(float)RAND_MAX)*(max-min);
}
  void DrawTorus(float radius,float width,float r,float g,float b) {
        int numc = 16, numt = 128;

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
					
					double dx=x-nx;
					double dy=y-ny;
					double dz=z-nz;

					double l=sqrt(dx*dx+dy*dy+dz*dz);
					
			 	glNormal3d(dx/l,dy/l,dz/l); 
					glColor3f(r,g,b);
                    glVertex3d( x,  z,  y);
					


                }
            }
            glEnd();
        }
    }
  float triSin(float x){
  
  return (sin(x)+sin(x*2)+sin(x*4))/3.0;
  }

  void randomDots(int m_dotcount){
  glBegin(GL_POINTS);
	
  
 float width=1000*m_dotcount;
 float height=1000*m_dotcount;
	  Vec3D current;
	  Vec3D current2;

  for(int i=0;i<width;i++){
  for(int j=0;j<height;j++){
	//  m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,0);

	  // Calc Color
	  hsv hsvvalue;
	  hsvvalue.h=360*(((i*width)+j)/(width*height));
	  hsvvalue.s=1;
	  hsvvalue.v=1;
	  rgb rgbresult=hsv2rgb(hsvvalue);


	  current.x=myRandom(-10,10);
current.y=myRandom(-10,10);
current.z=myRandom(-10,10);
glColor3f(rgbresult.r,rgbresult.g,rgbresult.b);
	  glVertex3f(current.x,current.y,current.z);
  

  }
  }
  glEnd();
	
  }

void FFGLHeat::createDisplayList(){
	// create one display list
	for(int i =0;i<125;i++){
	m_displayList[i] = glGenLists(1);

	// fill displaylist
	glNewList(m_displayList[i], GL_COMPILE);
 //

	if(i%2==0){
  DrawTorus((i+1)*1.0,0.25,0,1,1); 
	}else{
	DrawTorus((i+1)*1.0,0.25,1,0,0); 
	
	}
glEndList();
	}

}
DWORD FFGLHeat::DeInitGL()
{
  m_shader.FreeGLResources();

  if (renderedTexture1)
  {
    glDeleteTextures(1, &renderedTexture1);
    renderedTexture1 = 0;
  } 
  if (renderedTexture1Depth)
  {
	  glDeleteTextures(1, &renderedTexture1Depth);
    renderedTexture1Depth = 0;
  } 
  for(int i =0;i<torusCount;i++){
  glDeleteLists(m_displayList[i],1);
  }
  	  for(int i =0;i<torusCount;i++){
	  glDeleteLists(m_displayList[i],1);
	  }


    glDeleteRenderBuffersEXTPROC( m_FramebufferId);

  return FF_SUCCESS;
}

DWORD  FFGLHeat::SetTime(double time) {
	m_Time=time;
	return FF_SUCCESS; 
}


double systemTimeToDouble(SYSTEMTIME *time){
	return  ((double)time->wMinute*60000+ (double)time->wSecond*1000+(double)time->wMilliseconds)/(double)1000.0f;
}

double FFGLHeat::GetTimeInSecondsSinceStart(){
	 // Update Time Based on Systemtime and initial stored system time...
   SYSTEMTIME st;    
   GetSystemTime(&st); 

   double delta=systemTimeToDouble(&st)-systemTimeToDouble(&m_SystemTimeLastFrame);

   // increase time counter;
   timeCounter+=delta;

   // increase sin values
   times1.x+=delta*(speeds.x*8-4.0f);
   times1.y+=delta*(speeds.y*8-4.0f);
   times1.z+=delta*(speeds.z*8-4.0f);
   times2.x+=delta*(speeds2.x*8-4.0f);
   times2.y+=delta*(speeds2.y*8-4.0f);
   times2.z+=delta*(speeds2.z*8-4.0f);
   m_SystemTimeLastFrame=st;

   // decay adjustments, for smooth transitions
   decayRotCurrent.x=decayRotCurrent.x+(decayRot.x-decayRotCurrent.x)*.01;
   decayRotCurrent.y=decayRotCurrent.y+(decayRot.y-decayRotCurrent.y)*.01;
   decayRotCurrent.z=decayRotCurrent.z+(decayRot.z-decayRotCurrent.z)*.01;

   decayShiftCurrent.x=decayShiftCurrent.x+(decayShift.x-decayShiftCurrent.x)*.01;
   decayShiftCurrent.y=decayShiftCurrent.y+(decayShift.y-decayShiftCurrent.y)*.01;
   decayShiftCurrent.z=decayShiftCurrent.z+(decayShift.z-decayShiftCurrent.z)*.01;



//   return systemTimeToDouble(&st);
   return timeCounter;

}


void FFGLHeat::updateRenderTargets(int newWidth,int newHeight)
{
	if((newWidth!=lastWidth)||(newHeight!=lastHeight))
	{

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, renderedTexture1);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, newWidth, newHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		glBindTexture(GL_TEXTURE_2D, renderedTexture1Depth);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, newWidth, newHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		lastWidth=newWidth;
		lastHeight=newHeight;
	}
}



DWORD FFGLHeat::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
  if (pGL->numInputTextures<1) return FF_FAIL;
      glMatrixMode( GL_PROJECTION );
    
  if (pGL->inputTextures[0]==NULL) return FF_FAIL;

  
  glEnable( GL_TEXTURE_2D );
	updateRenderTargets(pGL->inputTextures[0]->Width,pGL->inputTextures[0]->Height);


	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FramebufferId) ;
	 
	m_extensions.glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderedTexture1, 0);

	m_extensions.glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderedTexture1Depth, 0);
	
	 glDepthMask(TRUE);

	/*
	 GLenum status;
  status = m_extensions.glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
 std::cout<< glGetError()<<"\n";
  switch(status)
  {
     case GL_FRAMEBUFFER_COMPLETE_EXT:
   
std::cout << "FRAMEBUFFER OK" <<status<< "\n";
break;
     case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
   
std::cout << "ERROR GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" <<status<< "\n";
break;

     case GL_FRAMEBUFFER_UNSUPPORTED:
   
std::cout << "ERROR GL_FRAMEBUFFER_UNSUPPORTED" <<status<< "\n";
break;

     case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
   
std::cout << "ERROR GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" <<status<< "\n";
break;

  default:
std::cout << "ERROR" <<status<< "\n";

  }
   */
  m_shader.BindShader();
  /*
  if(m_dotcount!=lastrounds){
  //rebuild display list
	  for(int i =0;i<torusCount;i++){
	  glDeleteLists(m_displayList[i],1);
	  }
	  createDisplayList();
  lastrounds=m_dotcount;
  }
  */
  // Set up camera 

  //activate our shader
  m_shader.BindShader();
//  glEnable(GL_POINT_SPRITE);
//  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
 // glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
  glDisable(GL_BLEND);
//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  //bind the heat texture to texture unit 0
  glEnable(GL_CULL_FACE ); 
  glFrontFace(GL_CW); 

	FFGLTextureStruct &Texture = *(pGL->inputTextures[0]);

	
	//get the max s,t that correspond to the 
	//width,height of the used portion of the allocated texture space
	FFGLTexCoords maxCoords = GetMaxGLTexCoords(Texture);

  //assign the maxCoords value in the shader
  //(glUniform2f assigns to a vec2)
  m_extensions.glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);
  
 
  m_extensions.glUniform3fARB(m_positionsLocation,position.x,position.y,position.z);
 

  //assign the heatAmount
 // m_extensions.glUniform1fARB(m_heatAmountLocation, 2.0*m_Heat);
  //assign the time
  m_extensions.glUniform1fARB(m_timeLocation,(float)GetTimeInSecondsSinceStart());
  //assign the BPM   
  //assign the BPM 
  m_extensions.glUniform1fARB(m_aspectLocation,((float)pGL->inputTextures[0]->Height/(float)pGL->inputTextures[0]->Width));

  
  m_extensions.glUniform1fARB(m_useTextureLocation,0.0); 

 glClearColor(0,0,0,0);
 glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  //activate texture unit 1 and bind the input texture
  m_extensions.glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, Texture.Handle);
    

  
  glMatrixMode(GL_PROJECTION);
  
   glLoadIdentity();
  gluPerspective(65,(float)pGL->inputTextures[0]->Width/(float)pGL->inputTextures[0]->Height, 0.1,1000);
  gluLookAt(4,2,0,0,0,0,0,1,0);
 // glRotatef(45,1,0,0);
   glMatrixMode(GL_MODELVIEW);


//glRotatef(rotation.y*360+position.y,0,0,1);
  //  glRotatef(rotation.y*360,0,1,0);
  //  glRotatef(rotation.z*360,0,0,1);

/*  glScalef(scale.x*1,scale.y*1,scale.z*1);

 	glTranslatef(translation.x*20-10,translation.y*20-10,translation.z*20-10);
   */
 //  glLoadIdentity();
// glRotatef(m_sinXSpeed*360,0,1,0);
 //glRotatef(m_sinXSpeed2*360,1,0,0);
  //draw the quad that will be painted by the shader/textures
  //note that we are sending texture coordinates to texture unit 1..
  //the vertex shader and fragment shader refer to this when querying for
  //texture coordinates of the inputTexture 
  
	  rotations[0].x=rotation.x*360+triSin(times1.x)*swayRot.x*180;
	  rotations[0].y=rotation.y*360+triSin(times1.y)*swayRot.y*180;
	  rotations[0].z=rotation.z*360+triSin(times1.z)*swayRot.z*180;

	  positions[0].x=(translation.x*2.0-1.0)*25.0+triSin(times2.x)*swayShift.x*10.0f;
	  positions[0].y=(translation.y*2.0-1.0)*25.0+triSin(times2.y)*swayShift.y*10.0f;
	  positions[0].z=(translation.z*2.0-1.0)*25.0+triSin(times2.z)*swayShift.z*10.0f;

	  for(int i =0;i<torusCount;i++){


	  if(i>0){
		  if(useFollow>0.5){
			  rotations[i].x=rotations[i].x+(rotations[i-1].x-rotations[i].x)*decayRotCurrent.x;
		  rotations[i].y=rotations[i].y+(rotations[i-1].y-rotations[i].y)*decayRotCurrent.y;
		  rotations[i].z=rotations[i].z+(rotations[i-1].z-rotations[i].z)*decayRotCurrent.z;
		  positions[i].x=positions[i].x+(positions[i-1].x-positions[i].x)*decayShiftCurrent.x;
		  positions[i].y=positions[i].y+(positions[i-1].y-positions[i].y)*decayShiftCurrent.y;
		  positions[i].z=positions[i].z+(positions[i-1].z-positions[i].z)*decayShiftCurrent.z;
		  }else{

	  rotations[i].x=rotation.x*360+triSin(times1.x+i*decayRotCurrent.x)*swayRot.x*180;
	  rotations[i].y=rotation.y*360+triSin(times1.y+i*decayRotCurrent.y)*swayRot.y*180;
	  rotations[i].z=rotation.z*360+triSin(times1.z+i*decayRotCurrent.z)*swayRot.z*180;

	  positions[i].x=(translation.x*2.0-1.0)*25.0+triSin(times2.x+i*decayShiftCurrent.x)*swayShift.x*10.0f;
	  positions[i].y=(translation.y*2.0-1.0)*25.0+triSin(times2.y+i*decayShiftCurrent.y)*swayShift.y*10.0f;
	  positions[i].z=(translation.z*2.0-1.0)*25.0+triSin(times2.z+i*decayShiftCurrent.z)*swayShift.z*10.0f;
		  }

	  }
	  /*
	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
  gluPerspective(65,(float)pGL->inputTextures[0]->Width/(float)pGL->inputTextures[0]->Height, 0.1,1000);
  gluLookAt(8,4,0,0,0,0,0,1,0);
  */
  glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
glTranslatef(positions[i].x,positions[i].y,positions[i].z);  
glRotatef(rotations[i].x, 1, 0, 0);
glRotatef(rotations[i].y, 0, 1, 0);
glRotatef(rotations[i].z, 0, 0, 1);
glCallList(m_displayList[i]);
  } 



 // m_shader.UnbindShader();
    m_extensions.glUniform1fARB(m_useTextureLocation,1.0); 

	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,  pGL->HostFBO) ;
  
	m_extensions.glActiveTexture(GL_TEXTURE1); 
glBindTexture(GL_TEXTURE_2D,renderedTexture1);

//  glBindTexture(GL_TEXTURE_2D, Texture.Handle);
    
 glClearColor(0,0,0,0);
 glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1,1,-1,1);
	glViewport(0,0,pGL->inputTextures[0]->Width,pGL->inputTextures[0]->Height);



renderQuad(1.0,1.0);
  /* 
	//upper left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0, maxCoords.t);
	glVertex2f(-1,1);

	//upper right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, maxCoords.s, maxCoords.t);
	glVertex2f(1,1);

	//lower right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, maxCoords.s, 0);
	glVertex2f(1,-1);
	glEnd();
	*/

  //unbind the input texture
  glBindTexture(GL_TEXTURE_2D, 0);
  
  //switch to texture unit 0 and unbind the 1D heat texture
  m_extensions.glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_1D, 0);
  
  //unbind the shader
  m_shader.UnbindShader();
  
	 glDepthMask(FALSE);
  return FF_SUCCESS;
}

DWORD FFGLHeat::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

	switch (dwIndex) {
		  
	default:
		return FF_FAIL;
	}
}

void FFGLHeat::renderQuad(float texMaxX,float texMaxY)
{

	glBegin(GL_QUADS);

	//lower left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,0);
	glVertex2f(-1,-1);

	//upper left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,texMaxY);
	glVertex2f(-1,1);

	//upper right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, texMaxX,texMaxY);
	glVertex2f(1,1);

	//lower right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1,texMaxX, 0);
	glVertex2f(1,-1);

	glEnd();

}
DWORD FFGLHeat::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam != NULL) {
		
		switch (pParam->ParameterNumber) {
  
		case FFPARAM_SPEED_ROT_X:
			speeds.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
		case FFPARAM_SPEED_ROT_Y:
			speeds.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
		case FFPARAM_SPEED_ROT_Z:
			speeds.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
  
		case FFPARAM_SPEED_SHIFT_X:
			speeds2.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
		case FFPARAM_SPEED_SHIFT_Y:
			speeds2.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
		case FFPARAM_SPEED_SHIFT_Z:
			speeds2.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
	 
		case FFPARAM_ROT_SWAY_X:
			swayRot.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
	 
		case FFPARAM_ROT_SWAY_Y:
			swayRot.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
	 
		case FFPARAM_ROT_SWAY_Z:
			swayRot.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_ROTX:
			rotation.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
	 
		case FFPARAM_ROTY:
			rotation.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
	 
		case FFPARAM_ROTZ:
			rotation.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
		case FFPARAM_TRANSLATEX:
			translation.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
		case FFPARAM_TRANSLATEY:
			translation.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
break;
	 
		case FFPARAM_TRANSLATEZ:
			translation.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 
	 
		case FFPARAM_TRANSLATE_SWAY_X:
			swayShift.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	 
		case FFPARAM_TRANSLATE_SWAY_Y:
			swayShift.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
break;
	 
		case FFPARAM_TRANSLATE_SWAY_Z:
			swayShift.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 

		case FFPARAM_DECAY:
			decay= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 

		case FFPARAM_DECAY_ROT_X:
			decayRot.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 
		case FFPARAM_DECAY_ROT_Y:
			decayRot.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 
		case FFPARAM_DECAY_ROT_Z:
			decayRot.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 

		case FFPARAM_DECAY_SHIFT_X:
			decayShift.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 
		case FFPARAM_DECAY_SHIFT_Y:
			decayShift.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 
		case FFPARAM_DECAY_SHIFT_Z:
			decayShift.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
		case FFPARAM_FOLLOW_OR_DECAY:
			useFollow= *((float *)(unsigned)&(pParam->NewParameterValue)); 

			break;
	 

	  
			break;

		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;
	
	}

	return FF_FAIL;
}
