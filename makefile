SOURCES  = *.c
INCLUDES = *.h
OBJECTS = binary_tree.o config.o passive.o tools.o transaction.o

CFLAGS = -Wall -Wextra -Wno-error
LDLIBS = -lpthread

all: Ex0

Ex0: $(OBJECTS)
	gcc $(SOURCES) -o ex0 $(CFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm -rf $(OBJECTS)
