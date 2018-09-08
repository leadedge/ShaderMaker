#include <stdio.h>
#include <dlfcn.h>

#include "../FFGLPluginInstance.h"
#include "../FFDebugMessage.h"

class LinuxPluginInstance :
public FFGLPluginInstance
{
public:
  LinuxPluginInstance();

  DWORD Load(const char *filename);
  DWORD Unload();

  virtual ~LinuxPluginInstance();

protected:
  void *plugin_handle;
};

FFGLPluginInstance *FFGLPluginInstance::New()
{
  return new LinuxPluginInstance();
}

LinuxPluginInstance::LinuxPluginInstance()
:plugin_handle(NULL)
{
}

DWORD LinuxPluginInstance::Load(const char *fname)
{
  if (fname==NULL || fname[0]==0)
    return FF_FAIL;

  Unload();

  plugin_handle = dlopen(fname, RTLD_NOW);
  if (plugin_handle == NULL)
  {
    printf("dlopen: %s\n", dlerror());
	Unload();
    return FF_FAIL;
  }

  FF_Main_FuncPtr pFreeFrameMain =
    (FF_Main_FuncPtr)(unsigned)dlsym(plugin_handle, "plugMain");

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

DWORD LinuxPluginInstance::Unload()
{
  DeinitPluginLibrary();

  if (plugin_handle != NULL)
  {
    if (!dlclose(plugin_handle))
    {
      plugin_handle = NULL;
    }
    else
    {
      FFDebugMessage("couldn't free dynamic library");
    }
  }

  return FF_SUCCESS;
}

LinuxPluginInstance::~LinuxPluginInstance()
{
  if (plugin_handle != NULL)
  {
    FFDebugMessage("plugin deleted without calling Unload()");
  }
}
