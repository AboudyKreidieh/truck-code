# Location of the base directory
BASE_DIR = $(shell pwd)/../../..
PATH_FROM_BASE = src/jbus

ARTIFACT = rd_j1939

# Build architecture/variant string, possible values: x86, armv7le, etc...
PLATFORM ?= x86_64

# Build profile, possible values: release, debug, profile, coverage
BUILD_PROFILE ?= debug

CONFIG_NAME ?= $(PLATFORM)-$(BUILD_PROFILE)
OUTPUT_DIR = $(BASE_DIR)/build/$(CONFIG_NAME)/$(PATH_FROM_BASE)
TARGET = $(BASE_DIR)/build/$(CONFIG_NAME)/$(ARTIFACT)

# Compiler definitions
CC = qcc -Vgcc_nto$(PLATFORM)
CXX = qcc -lang-c++ -Vgcc_nto$(PLATFORM)
LD = $(CXX)

# User defined include/preprocessor flags and libraries
INCLUDES += -I../

#LIBS += -L/path/to/my/lib/$(PLATFORM)/usr/lib -lmylib
#LIBS += -L$(OUTPUT_DIR)/../utils -ltimestamp

# Compiler flags for build profiles
CCFLAGS_release += -O2
CCFLAGS_debug += -g -O0 -fno-builtin
CCFLAGS_coverage += -g -O0 -ftest-coverage -fprofile-arcs -nopipe -Wc,-auxbase-strip,$@
LDFLAGS_coverage += -ftest-coverage -fprofile-arcs
CCFLAGS_profile += -g -O0 -finstrument-functions
LIBS_profile += -lprofilingS

# Generic compiler flags (which include build type flags)
CCFLAGS_all += -Wall -fmessage-length=0
CCFLAGS_all += $(CCFLAGS_$(BUILD_PROFILE))
# Shared library has to be compiled with -fPIC
#CCFLAGS_all += -fPIC
LDFLAGS_all += $(LDFLAGS_$(BUILD_PROFILE))
LIBS_all += $(LIBS_$(BUILD_PROFILE)) -lpps
DEPS = -Wp,-MMD,$(@:%.o=%.d),-MT,$@

# Source list
SRCS := $(shell find -name '*.cpp' -or -name '*.c' -or -name '*.s' -and -not -name 'translate_pdu.cpp')
#SRC = $(wildcard *.cpp)

# Object files list
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))
#OBJS := $(addsuffix .o,$(basename $(SRCS)))
mains = $(OUTPUT_DIR)/translate_pdu.o
SRC_FILES := $(wildcard *.cpp)
SRC_FILES := $(filter-out translate_pdu.cpp, $(SRC_FILES))

#Compiling rule
$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<
for file in $(SRC_FILES); do \
#	$(OUTPUT_DIR)/%.o: file
	@mkdir -p $(dir $@) \
	$(CXX) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS); \
	done
#$(OUTPUT_DIR)/$(filter-out $(mains),$(OBS)): %.cpp
#	@echo $(SRC_FILES)

#Linking rule
# TODO(ak): see if I can fix this hack
$(TARGET):$(OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $(TARGET) $(LDFLAGS_all) $(LDFLAGS) $(OBJS) $(LIBS_all) $(LIBS) $(OUTPUT_DIR)/../utils/timestamp.o

#Rules section for default compilation and linking
all: $(TARGET)

clean:
	rm -fr $(OUTPUT_DIR)

rebuild: clean all

#Inclusion of dependencies (object files to source and includes)
-include $(OBJS:%.o=%.d)