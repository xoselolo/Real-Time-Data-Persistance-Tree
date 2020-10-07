SOURCES  = *.c
INCLUDES = *.h


CFLAGS = -Wall -Wextra
LDLIBS = -lpthread

all: Ex0

Ex0: $(OBJECTS)
	gcc $(SOURCES) -o ex0 $(CFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm -rf $(OBJECTS)
