# Location of the base directory
BASE_DIR = $(shell pwd)/..
PATH_FROM_BASE = tests

# Build architecture/variant string, possible values: x86, armv7le, etc...
PLATFORM ?= x86_64

# Build profile, possible values: release, debug, profile, coverage
BUILD_PROFILE ?= debug

CONFIG_NAME ?= $(PLATFORM)-$(BUILD_PROFILE)
OUTPUT_DIR = $(BASE_DIR)/build/$(CONFIG_NAME)/$(PATH_FROM_BASE)

# Compiler definitions
CC = qcc -Vgcc_nto$(PLATFORM)
CXX = qcc -lang-c++ -Vgcc_nto$(PLATFORM)

# User defined include/preprocessor flags and libraries
INCLUDES += -I../include
INCLUDES += -I../thirdparty/boost/include

# Generic compiler flags (which include build type flags)
CCFLAGS_all += -Wall -fmessage-length=0
CCFLAGS_all += $(CCFLAGS_$(BUILD_PROFILE))
DEPS = -Wp,-MMD,$(@:%.o=%.d),-MT,$@
LIBS += -l pps -l stdc++

# Source list
SRCS := $(shell find -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Object files list
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))

# Target files list
TARGETS = $(OUTPUT_DIR)/../run_tests

# Objects to be included while linking
OBJECTS = $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/jbus/*.o) 
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/utils/timestamp.o)
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/utils/sys.o)
#OBJECTS += $(wildcard ../thirdparty/boost/lib//*)
OBJECTS += ../thirdparty/boost/lib/libboost_unit_test_framework.a

# Compiling rule
$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<
$(OUTPUT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<

# Linking rule
$(OUTPUT_DIR)/bin/test_j1939_interpreters $(OUTPUT_DIR)/bin/test_translate_pdu : $(OUTPUT_DIR)/test_j1939_interpreters.o $(OUTPUT_DIR)/test_translate_pdu.o
	@mkdir -p $(dir $@)
	$(LD) -o $(OUTPUT_DIR)/bin/test_j1939_interpreters $(OUTPUT_DIR)/test_j1939_interpreters.o $(LIBS) $(OBJECTS)
	$(LD) -o $(OUTPUT_DIR)/bin/test_translate_pdu $(OUTPUT_DIR)/test_translate_pdu.o $(LIBS) $(OBJECTS)
#	$(LD) -o $(OUTPUT_DIR)/bin/test_j1939_utils $(OUTPUT_DIR)/test_j1939_utils.o $(LIBS) $(OBJECTS)

# Rules section for default compilation and linking
all: $(OUTPUT_DIR)/bin/test_j1939_interpreters $(OUTPUT_DIR)/bin/test_translate_pdu

#$(TARGETS): $(OBJS)
#	@mkdir -p $(dir $@)
#	$(LD) -o $@ $< $(LIBS) $(OBJECTS)
#
## Rules section for default compilation and linking
#all: $(TARGETS)

clean:
	rm -fr $(OUTPUT_DIR)

rebuild: clean all

# Inclusion of dependencies (object files to source and includes)
-include $(OBJS:%.o=%.d)
