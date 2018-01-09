#ifndef FFGLShader_H
#define FFGLShader_H

#include <FFGL.h>
#include <FFGLExtensions.h>

class FFGLShader
{
public:
  FFGLShader();
  virtual ~FFGLShader();

  void SetExtensions(FFGLExtensions *e);
  
  int IsReady() { return (m_glProgram!=0 && m_glVertexShader!=0 && m_glFragmentShader!=0 && m_linkStatus==1); }
  
  int Compile(const char *vtxProgram, const char *fragProgram);

  GLuint FindUniform(const char *name);
  
  int BindShader();
  int UnbindShader();

  void FreeGLResources();
protected:

	
  FFGLExtensions *m_extensions;
  GLuint m_linkStatus; 
private:
  GLenum m_glVertexShader;
  GLenum m_glFragmentShader;
  GLenum m_glProgram;
  void CreateGLResources();
};

#endif
