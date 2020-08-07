
COMPONENT_ADD_INCLUDEDIRS := lib/include \
                             wake_word_engine/include \
							 acoustic_algorithm/include \
	                 

LIB_FILES := $(shell ls $(COMPONENT_PATH)/wake_word_engine/lib*.a) \
	         $(shell ls $(COMPONENT_PATH)/lib/lib*.a) \
	         $(shell ls $(COMPONENT_PATH)/acoustic_algorithm/lib*.a) \

LIBS := $(patsubst lib%.a,-l%,$(LIB_FILES))

COMPONENT_ADD_LDFLAGS +=  -L$(COMPONENT_PATH)/lib \
                          -L$(COMPONENT_PATH)/wake_word_engine \
                          -L$(COMPONENT_PATH)/acoustic_algorithm \
						  $(LIBS)
