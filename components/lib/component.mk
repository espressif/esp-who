COMPONENT_ADD_INCLUDEDIRS := include

COMPONENT_SRCDIRS := .

LIBS := coefficients dl_lib

COMPONENT_ADD_LDFLAGS +=  -L$(COMPONENT_PATH)/ $(addprefix -l,$(LIBS))

ALL_LIB_FILES += $(patsubst %,$(COMPONENT_PATH)/lib%.a,$(LIBS))
