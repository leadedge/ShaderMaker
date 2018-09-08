#include "../FFGLPluginInstance.h"
#include <FFDebugMessage.h>
#include <stdio.h>
#include <mach-o/dyld.h>

class OSXPluginInstance :
public FFGLPluginInstance
{
public:
  OSXPluginInstance();
  
  DWORD Load(const char *filename);
  DWORD Unload();
  
  virtual ~OSXPluginInstance();

protected:
  NSObjectFileImage m_ffImage;
  NSModule m_ffModule;
};

FFGLPluginInstance *FFGLPluginInstance::New()
{
  return new OSXPluginInstance();
}

OSXPluginInstance::OSXPluginInstance()
:m_ffImage(NULL),
 m_ffModule(NULL)
{
}

DWORD OSXPluginInstance::Load(const char *fname)
{
  if (fname==NULL || fname[0]==0)
    return FF_FAIL;

  Unload();
  
  if (NSCreateObjectFileImageFromFile(fname, &m_ffImage)!=NSObjectFileImageSuccess)
    return FF_FAIL;

  NSModule m_ffModule =
    NSLinkModule(
      m_ffImage,
      fname,
      NSLINKMODULE_OPTION_NONE);
      
  if (m_ffModule==NULL)
  {
    Unload(); //to undo NSCreateObjectFileImageFromFile
    return FF_FAIL;
  }
  
  NSSymbol s = NSLookupSymbolInModule(m_ffModule, "_plugMain");
  if (s==NULL)
  {
    Unload();//to undo NSLinkModule and NSCreateObjectFileImageFromFile
    return FF_FAIL;
  }
  
	FF_Main_FuncPtr pFreeFrameMain = (FF_Main_FuncPtr)NSAddressOfSymbol(s);

	if (pFreeFrameMain==NULL)
  {
    Unload(); //to undo same
    return FF_FAIL;
  }
  
  m_ffPluginMain = pFreeFrameMain;
  
  DWORD rval = InitPluginLibrary();
  if (rval!=FF_SUCCESS)
    return rval;
  
  return FF_SUCCESS;
}

DWORD OSXPluginInstance::Unload()
{
  if (m_ffInstanceID!=INVALIDINSTANCE)
  {
    FFDebugMessage("Failed to call DeInstantiateGL before Unload()");
    return FF_FAIL;
  }
  
  DeinitPluginLibrary();
  
	if (m_ffModule != NULL)
  {
    if (NSUnLinkModule(m_ffModule, NSUNLINKMODULE_OPTION_NONE))
    {
      m_ffModule = NULL;
    }
    else
    {
      FFDebugMessage("couldn't free dynamic library");
    }
  }

  if (m_ffImage != NULL)
  {
    if (NSDestroyObjectFileImage(m_ffImage))
    {
      m_ffImage = NULL;
    }
    else
    {
      FFDebugMessage("couldn't destroy object file image");
    }
  }

  return FF_SUCCESS;
}

OSXPluginInstance::~OSXPluginInstance()
{
  if (m_ffModule!=NULL)
  {
    FFDebugMessage("plugin deleted without calling Unload()");
  }
}
