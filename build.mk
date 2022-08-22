# Makefile 
# This is for building .nso and .npdm
# The content in build directory are managed by this make file
# The other wrapper scripts are contained in Justfile

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# Variables

# Target name
TARGET          := botwsavs

# DIRECTORIES
# Build Output (This mk file operates inside the build folder)
BUILD_DIR 		:= .
# Root Directory
ROOT_DIR        := ..
# Dependencies
LIB_DIR         := $(ROOT_DIR)/lib
# Our Sources
SOURCE_DIR      := $(ROOT_DIR)/src/
# Config files. These are needed to link
CONFIG_DIR		:= $(ROOT_DIR)/config
# Skyline (the injection framework) source
#SKYLINE_SRC		:=  $(LIB_DIR)/locked/skyline/src

# Scan for nested source directories
ALL_SOURCES_DIRS	:= 	$(shell find $(SOURCE_DIR) -type d)
# Library paths
LIBDIRS :=  $(PORTLIBS) $(LIBNX)
# Include paths
ALL_INCLUDE_DIRS	:=	\
$(SOURCE_DIR) \
${LIB_DIR}/nnheaders/include \
$(LIB_DIR)/sead/include  \
$(LIB_DIR)/botw
# $(LIB_DIR)/locked/skyline/include 
# $(LIB_DIR)/locked/libeiffel/include 
# $(LIB_DIR)/sync/agl/include 


# ${LIB_DIR}/sync/xlink2/include 
# ${LIB_DIR}/sync/botw/src

# VPATH for make to search for files
VPATH	:=	$(foreach dir,$(ALL_SOURCES_DIRS),$(CURDIR)/$(dir))

# Linker script for uking symbols
LDSCRIPT    := $(CONFIG_DIR)/symbols.ld
# Linker version script
LINKER_VERSION_SCRIPT := $(CONFIG_DIR)/exported
# .specs file for linking
SWITCH_SPECS := $(CONFIG_DIR)/switch.specs
# Source files 
CFILES		:=	$(foreach dir,$(ALL_SOURCES_DIRS),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(ALL_SOURCES_DIRS),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(ALL_SOURCES_DIRS),$(notdir $(wildcard $(dir)/*.s)))

# OUTPUT FILES

# .o files
OFILES	 :=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
# .d files 
DFILES	 :=	$(OFILES:.o=.d)
# Application json for generating npdm
APP_JSON := $(CONFIG_DIR)/app.json

# CODE GEN OPTIONS
# Use CXX for linking
LD	    := $(CXX)
# Include path
INCLUDE	:=	\
$(foreach dir,$(ALL_INCLUDE_DIRS),-I$(CURDIR)/$(dir)) \
$(foreach dir,$(LIBDIRS),-I$(dir)/include)
# Defines
DEFINES := -D__SWITCH__ -DSWITCH -DNNSDK $(EXTRA_DEFINES) 
# Architecture
ARCH	:= -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIC -ftls-model=local-exec
# C flags
CFLAGS	:= -g -Wall -ffunction-sections -O3 $(ARCH) $(DEFINES) $(INCLUDE) 
# CXX flags
CXXFLAGS	:= $(CFLAGS) -fno-rtti -fomit-frame-pointer -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables -enable-libstdcxx-allocator=new -fpermissive -std=c++17 
# AS flags
ASFLAGS	    := -g $(ARCH)
# LD flags
LDFLAGS     := -specs=$(SWITCH_SPECS) -g $(ARCH) -Wl,-Map,$(TARGET).map -Wl,--version-script=$(LINKER_VERSION_SCRIPT) -Wl,-init=__custom_init -Wl,-fini=__custom_fini -Wl,--export-dynamic -nodefaultlibs
# LD libs
LIBS	    := -lgcc -lstdc++ -u malloc
# LD lib paths
LIBPATHS	:= $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
# DEPSDIR used by DEVKITPRO for exporting .d files
DEPSDIR	    ?= .

#---------------------------------------------------------------------------------
# Make Targets
.PHONY:	all
all: $(TARGET).nso app.npdm

# Make target ELF depend on all .o files
$(TARGET).elf   : $(OFILES) $(SWITCH_SPECS)

# Not sure why the default npdm rule fails. Redefining the rule here.
# The tool prints error message for missing fields in json. They are not important so we ignore the errors
app.npdm: $(APP_JSON)
	npdmtool $(APP_JSON) $@ 2> /dev/null
# The rest of the build rules are specified by the devkitpro makefile

# Include the .d files generated
-include $(DFILES)
