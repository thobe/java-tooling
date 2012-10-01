# Mac OS X only for now
INCLUDES=-I"$(JAVA_HOME)/include" \
         -I"$(HEADERS)"

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:%.c=$(TARGET)/%.o)

$(OUT)/libjavatooling.jnilib : $(OBJECTS)
	cc -dynamiclib -o $@ $(OBJECTS) -framework JavaVM

$(TARGET)/%.o : %.c
	cc $(INCLUDES) -c $< -o $@
