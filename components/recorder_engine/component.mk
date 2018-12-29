COMPONENT_ADD_INCLUDEDIRS := . \
                             ./include

COMPONENT_SRCDIRS := . \
                     ./include

LIBS := esp_wakenet nn_model

COMPONENT_ADD_LDFLAGS := -L$(COMPONENT_PATH)/ $(addprefix -l,$(LIBS))

ALL_LIB_FILES += $(patsubst %,$(COMPONENT_PATH)/lib%.a,$(LIBS))

