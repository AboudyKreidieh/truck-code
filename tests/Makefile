include ../Makefile.variable

# Desired location of the output binaries.
PATH_FROM_BASE = tests
CONFIG_NAME ?= $(PLATFORM)-$(BUILD_PROFILE)
OUTPUT_DIR = $(BASE_DIR)/build/$(CONFIG_NAME)/$(PATH_FROM_BASE)

# User defined include/preprocessor flags and libraries
INCLUDES += -I../thirdparty/boost/include

# Source list
SRCS := $(shell find -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Object files list
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))

# Target files list
TARGETS = $(OUTPUT_DIR)/../run_tests

# Objects to be included while linking
OBJECTS = $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/jbus/*.o) 
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/can/*.o) 
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/logger/*.o)
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/utils/*.o)
OBJECTS += $(wildcard $(BASE_DIR)/build/$(CONFIG_NAME)/include/tests/*.o)
#OBJECTS += $(wildcard ../thirdparty/boost/lib//*)
OBJECTS += ../thirdparty/boost/lib/libboost_unit_test_framework.a

# Compiling rule
$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -fprofile-arcs -ftest-coverage -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<
$(OUTPUT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -fprofile-arcs -ftest-coverage -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<

# Linking rule
$(OUTPUT_DIR)/bin/test_j1939_interpreters $(OUTPUT_DIR)/bin/test_logger $(OUTPUT_DIR)/bin/test_pubsub $(OUTPUT_DIR)/bin/test_translate_pdu : $(OUTPUT_DIR)/test_j1939_interpreters.o $(OUTPUT_DIR)/test_logger.o $(OUTPUT_DIR)/test_pubsub.o $(OUTPUT_DIR)/test_translate_pdu.o
	@mkdir -p $(dir $@)
	$(LD) -fprofile-arcs -o $(OUTPUT_DIR)/bin/test_j1939_interpreters $(OUTPUT_DIR)/test_j1939_interpreters.o $(LIBS) $(OBJECTS)
	$(LD) -fprofile-arcs -o $(OUTPUT_DIR)/bin/test_translate_pdu $(OUTPUT_DIR)/test_translate_pdu.o $(LIBS) $(OBJECTS)
	$(LD) -fprofile-arcs -o $(OUTPUT_DIR)/bin/test_logger $(OUTPUT_DIR)/test_logger.o $(LIBS) $(OBJECTS)
	$(LD) -fprofile-arcs -o $(OUTPUT_DIR)/bin/test_pubsub $(OUTPUT_DIR)/test_pubsub.o $(LIBS) $(OBJECTS)

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
