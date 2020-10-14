#
# Main Makefile. This is basically the same as a component makefile.
#

hid_device_le_prf.o: CFLAGS += -Wno-unused-const-variable 

COMPONENT_SRCDIRS += ../components/Mouse_Motion/
COMPONENT_ADD_INCLUDEDIRS += ../components/Mouse_Motion/
