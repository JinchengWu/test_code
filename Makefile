CC=${CROSS}-gcc
CFLAGS= -g -Wall -pthread
LD=${CROSS}-ld
LDFLAGS=
DEPENDFLAG=
INCLUDES=./
LIBS=
CFLAGS:=$(CFLAGS) -I$(INCLUDES)
LDFLAGS:=$(LDFLAGS) $(LIBS)
SRCDIRS=./
PROGRAMS=test
XML=*.xml
SHL=sh
BUILD_DIR?=./build
SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(PROGRAMS): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS) $(DEPENDS) $(BUILD_DIR)/$(PROGRAMS)
