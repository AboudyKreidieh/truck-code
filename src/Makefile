include ../Makefile.variable

# Desired location of the output binaries.
PATH_FROM_BASE = src
CONFIG_NAME ?= $(PLATFORM)-$(BUILD_PROFILE)
OUTPUT_DIR = $(BASE_DIR)/build/$(CONFIG_NAME)/$(PATH_FROM_BASE)

# Source list
SRCS := $(shell find -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Object files list
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))

# object to be included while linking
OBJECTS = $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/jbus/*.o) 
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/can/*.o) 
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/logger/*.o)
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/utils/*.o)

# Compiling rule
$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<
$(OUTPUT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<

# Linking rule
$(OUTPUT_DIR)/../can_man $(OUTPUT_DIR)/../translate_pdu $(OUTPUT_DIR)/../rd_j1939 $(OUTPUT_DIR)/../log: $(OUTPUT_DIR)/translate_pdu.o $(OUTPUT_DIR)/rd_j1939.o $(OUTPUT_DIR)/can_man.o $(OUTPUT_DIR)/log.o
	@mkdir -p $(dir $@)
	$(LD) -o $(OUTPUT_DIR)/../translate_pdu $(OUTPUT_DIR)/translate_pdu.o $(LIBS) $(OBJECTS)
	$(LD) -o $(OUTPUT_DIR)/../rd_j1939 $(OUTPUT_DIR)/rd_j1939.o $(LIBS) $(OBJECTS)
	$(LD) -o $(OUTPUT_DIR)/../can_man $(OUTPUT_DIR)/can_man.o $(LIBS) $(OBJECTS)
	$(LD) -o $(OUTPUT_DIR)/../log $(OUTPUT_DIR)/log.o $(LIBS) $(OBJECTS)

# Rules section for default compilation and linking
all: $(OUTPUT_DIR)/../translate_pdu $(OUTPUT_DIR)/../rd_j1939 $(OUTPUT_DIR)/../can_man $(OUTPUT_DIR)/../log

clean:
	rm -fr $(OUTPUT_DIR)

rebuild: clean all

# Inclusion of dependencies (object files to source and includes)
-include $(OBJS:%.o=%.d)
