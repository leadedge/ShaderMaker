DIR_SRC = ../../../Source
DIR_HOST = $(DIR_SRC)/FFGLHost/Linux
DIR_COMMON = $(DIR_SRC)/Common

DEBUG = 0

MAIN  = FFGLHost
SRCS  = $(DIR_HOST)/LinuxDebugMessage.cpp $(DIR_HOST)/LinuxPluginInstance.cpp \
	$(DIR_HOST)/LinuxTimer.cpp $(DIR_HOST)/../FFGLPluginInstance.cpp \
	$(DIR_COMMON)/FFGLExtensions.cpp $(DIR_COMMON)/FFGLFBO.cpp \
	$(DIR_HOST)/LinuxMain.cpp

OBJS = $(notdir $(SRCS:%cpp=%o))

vpath %.cpp $(DIR_SRC):$(DIR_COMMON):$(DIR_HOST):$(DIR_HOST)/..

CCPP = @g++
CPPFLAGS = -Wall -Wno-unknown-pragmas -pedantic \
	-I../../../Include

LDFLAGS = -lX11 -lGL -lglut

ifeq ($(DEBUG), 1)
	CPPFLAGS += -ggdb2 -O0 -D_DEBUG=1
else
	CPPFLAGS += -g0 -O3
endif


all: $(MAIN)

%.o: %.cpp
	$(CCPP) -c $(CPPFLAGS) -o $@ $<
	
$(MAIN): $(OBJS)
	$(CCPP) -o $@ $(OBJS) $(LDFLAGS)

.PHONY: clean run

clean:
	-rm -rf $(MAIN) $(OBJS)

run: $(MAIN)
	./$(MAIN)
