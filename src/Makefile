# Location of the base directory
BASE_DIR = $(shell pwd)/..
PATH_FROM_BASE = src

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
INCLUDES += -I../thirdparty/eigen3

# Generic compiler flags (which include build type flags)
CCFLAGS_all += -Wall -fmessage-length=0
CCFLAGS_all += $(CCFLAGS_$(BUILD_PROFILE))
DEPS = -Wp,-MMD,$(@:%.o=%.d),-MT,$@
LIBS += -lpps

# Source list
SRCS := $(shell find -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Object files list
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))

# object to be included while linking
OBJECTS = $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/jbus/*.o) 
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/can/*.o) 
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/utils/timestamp.o)
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/utils/sys.o)

# Compiling rule
$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<
$(OUTPUT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<

# Linking rule
$(OUTPUT_DIR)/../can_man $(OUTPUT_DIR)/../translate_pdu $(OUTPUT_DIR)/../rd_j1939: $(OUTPUT_DIR)/translate_pdu.o $(OUTPUT_DIR)/rd_j1939.o $(OUTPUT_DIR)/can_man.o
	@mkdir -p $(dir $@)
	$(LD) -o $(OUTPUT_DIR)/../translate_pdu $(OUTPUT_DIR)/translate_pdu.o $(LIBS) $(OBJECTS)
	$(LD) -o $(OUTPUT_DIR)/../rd_j1939 $(OUTPUT_DIR)/rd_j1939.o $(LIBS) $(OBJECTS)
	$(LD) -o $(OUTPUT_DIR)/../can_man $(OUTPUT_DIR)/can_man.o $(LIBS) $(OBJECTS)

# Rules section for default compilation and linking
all: $(OUTPUT_DIR)/../translate_pdu $(OUTPUT_DIR)/../rd_j1939 $(OUTPUT_DIR)/../can_man

clean:
	rm -fr $(OUTPUT_DIR)

rebuild: clean all

# Inclusion of dependencies (object files to source and includes)
-include $(OBJS:%.o=%.d)
