CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++

GSL_DIR = ./gsl
GSL_BUILD_DIR = $(GSL_DIR)/build
NO_AMPL_BINDINGS = 1
CPPFLAGS = -I$(GSL_BUILD_DIR)
CFLAGS = -g -std=gnu99 -O1 -Wall
CXXFLAGS = -g -std=gnu++11 -O1 -Wall
LDFLAGS += -static -L$(GSL_BUILD_DIR) -lgsl -lgslcblas  -lm 
LDLIBS += -lrt -lpthread -lgsl -lgslcblas  -lm 

SOURCES = main.c scanner.c mzapo_phys.c mzapo_parlcd.c serialize_lock.c lcd.c bmp_reader.c knob.c menu.c font_prop14x16.c
TARGET_EXE = main

ifeq ($(TARGET_IP),)
ifneq ($(filter debug run,$(MAKECMDGOALS)),)
$(warning The target IP address is not set)
$(warning Run as "TARGET_IP=192.168.223.106 make run" or modify Makefile)
TARGET_IP = 192.168.223.106
endif
endif
TARGET_DIR ?= /tmp/$(shell whoami)
TARGET_USER ?= root

OBJECTS = $(SOURCES:.c=.o)

ifeq ($(filter %.cpp,$(SOURCES)),)
LINKER = $(CC)
LDFLAGS += $(CFLAGS) $(CPPFLAGS)
else
LINKER = $(CXX)
LDFLAGS += $(CXXFLAGS) $(CPPFLAGS)
endif
all:  $(TARGET_EXE) 

gsl_build:
	@export CC=$(CC) && export CXX=$(CXX)
	@echo "Building GSL library..."
	mkdir -p $(GSL_BUILD_DIR)
	cd $(GSL_BUILD_DIR) && cmake .. -G"Unix Makefiles" -DNO_AMPL_BINDINGS=$(NO_AMPL_BINDINGS)
	cd $(GSL_BUILD_DIR) && $(MAKE)

%.o: %.c gsl_build
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cpp gsl_build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<



$(TARGET_EXE):  $(OBJECTS)
	$(LINKER) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: dep all run copy-executable debug clean

dep: depend

depend: $(SOURCES) *.h
	echo '# autogenerated dependencies' > depend
ifneq ($(filter %.c,$(SOURCES)),)
	$(CC) $(CFLAGS) $(CPPFLAGS) -w -E -M $(filter %.c,$(SOURCES)) >> depend
endif
ifneq ($(filter %.cpp,$(SOURCES)),)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -w -E -M $(filter %.cpp,$(SOURCES)) >> depend
endif

clean:
	rm -f *.o *.a $(OBJECTS) $(TARGET_EXE) connect.gdb depend
#	rm -rf $(GSL_BUILD_DIR)

copy-executable: $(TARGET_EXE)
	ssh $(SSH_OPTIONS) -t $(TARGET_USER)@$(TARGET_IP) killall gdbserver 1>/dev/null 2>/dev/null || true
	ssh $(SSH_OPTIONS) $(TARGET_USER)@$(TARGET_IP) mkdir -p $(TARGET_DIR)
	scp $(SSH_OPTIONS) $(TARGET_EXE) $(TARGET_USER)@$(TARGET_IP):$(TARGET_DIR)/$(TARGET_EXE)
	scp $(SSH_OPTIONS) "./*.bmp" $(TARGET_USER)@$(TARGET_IP):$(TARGET_DIR)/
run: copy-executable $(TARGET_EXE)
	ssh $(SSH_OPTIONS) -t $(TARGET_USER)@$(TARGET_IP) $(TARGET_DIR)/$(TARGET_EXE)

ifneq ($(filter -o ProxyJump=,$(SSH_OPTIONS))$(SSH_GDB_TUNNEL_REQUIRED),)
SSH_GDB_PORT_FORWARD = -L 12345:127.0.0.1:12345
TARGET_GDB_PORT = 127.0.0.1:12345
else
TARGET_GDB_PORT = $(TARGET_IP):12345
endif

debug: copy-executable
	xterm -e ssh $(SSH_OPTIONS) $(SSH_GDB_PORT_FORWARD) -t $(TARGET_USER)@$(TARGET_IP) gdbserver :12345 $(TARGET_DIR)/$(TARGET_EXE) &
	sleep 2
	echo >connect.gdb "target extended-remote $(TARGET_GDB_PORT)"
	echo >>connect.gdb "b main"
	echo >>connect.gdb "c"
	ddd --debugger gdb-multiarch -x connect.gdb $(TARGET_EXE)

-include depend