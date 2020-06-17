#Component makefile

COMPONENT_ADD_INCLUDEDIRS := include

#COMPONENT_EXTRA_INCLUDES : 

COMPONENT_SRCDIRS := .

LIB_FILES := $(shell ls ../lib/lib*.a)

LIBS := $(patsubst lib%.a,-l%,$(notdir $(LIB_FILES)))

COMPONENT_ADD_LDFLAGS +=  -L$(COMPONENT_PATH)/ $(LIBS)

ALL_LIB_FILES += $(LIB_FILES)
