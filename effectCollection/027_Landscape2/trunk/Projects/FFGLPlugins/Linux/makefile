DIR_SRC = ../../../Source/FFGLPlugins
DIR_COMMON = ../../../Source/Common

DEBUG = 0

FFGLBRIGHTNESS_SRCS = $(DIR_SRC)/FFGLBrightness/FFGLBrightness.cpp
FFGLTILE_SRCS = $(DIR_SRC)/FFGLTile/FFGLTile.cpp
FFGLHEAT_SRCS = $(DIR_SRC)/FFGLHeat/FFGLHeat.cpp
FFGLLUMAKEY_SRCS = $(DIR_SRC)/FFGLLumaKey/FFGLLumaKey.cpp
FFGLGRADIENTS_SRCS = $(DIR_SRC)/FFGLGradients/FFGLGradients.cpp
FFGLTIME_SRCS = $(DIR_SRC)/FFGLTime/FFGLTime.cpp

FFGLBRIGHTNESS_OBJS = $(notdir $(FFGLBRIGHTNESS_SRCS:%cpp=%o))
FFGLTILE_OBJS = $(notdir $(FFGLTILE_SRCS:%cpp=%o))
FFGLHEAT_OBJS = $(notdir $(FFGLHEAT_SRCS:%cpp=%o))
FFGLLUMAKEY_OBJS = $(notdir $(FFGLLUMAKEY_SRCS:%cpp=%o))
FFGLGRADIENTS_OBJS = $(notdir $(FFGLGRADIENTS_SRCS:%cpp=%o))
FFGLTIME_OBJS = $(notdir $(FFGLTIME_SRCS:%cpp=%o))

COMMON_SRCS = $(DIR_SRC)/FFGLPluginInfo.cpp $(DIR_SRC)/FFGLPluginInfoData.cpp \
	$(DIR_SRC)/FFGL.cpp $(DIR_COMMON)/FFGLShader.cpp \
	$(DIR_COMMON)/FFGLExtensions.cpp \
	$(DIR_SRC)/FFGLPluginManager.cpp $(DIR_SRC)/FFGLPluginSDK.cpp

COMMON_OBJS = $(notdir $(COMMON_SRCS:%cpp=%o))

OBJS = $(COMMON_OBJS) $(FFGLBRIGHTNESS_OBJS) $(FFGLTILE_OBJS) $(FFGLHEAT_OBJS) \
	$(FFGLLUMAKEY_OBJS) $(FFGLGRADIENTS_OBJS) $(FFGLTIME_OBJS)

vpath %.cpp $(DIR_SRC)/FFGLBrightness:$(DIR_SRC)/FFGLTile:$(DIR_SRC)/FFGLHeat: \
	$(DIR_SRC)/FFGLLumaKey:$(DIR_SRC)/FFGLGradients: \
	$(DIR_SRC)/FFGLTime:$(DIR_SRC):$(DIR_COMMON)

CCPP = @g++
CPPFLAGS = -Wall -Wno-unknown-pragmas -pedantic -fPIC \
	-I../../../Include

CSHLIB = $(CCPP) $(CPPFLAGS) -shared -Wl,-soname,$@ -o $@ $^ \
	-lc -lX11 -lGL -lglut

ifeq ($(DEBUG), 1)
	CPPFLAGS += -ggdb2 -O0 -D_DEBUG=1
else
	CPPFLAGS += -g0 -O3
endif

all: FFGLBrightness.so FFGLTile.so FFGLHeat.so FFGLLumaKey.so FFGLGradients.so \
	FFGLTime.so

%.o: %.cpp
	$(CCPP) -c $(CPPFLAGS) -o $@ $<

FFGLBrightness.so: $(FFGLBRIGHTNESS_OBJS) $(COMMON_OBJS)
	$(CSHLIB)

FFGLTile.so: $(FFGLTILE_OBJS) $(COMMON_OBJS)
	$(CSHLIB)

FFGLHeat.so: $(FFGLHEAT_OBJS) $(COMMON_OBJS)
	$(CSHLIB)

FFGLLumaKey.so: $(FFGLLUMAKEY_OBJS) $(COMMON_OBJS)
	$(CSHLIB)

FFGLGradients.so: $(FFGLGRADIENTS_OBJS) $(COMMON_OBJS)
	$(CSHLIB)

FFGLTime.so: $(FFGLTIME_OBJS) $(COMMON_OBJS)
	$(CSHLIB)

.PHONY: clean

clean:
	-rm -rf $(OBJS) *.so

