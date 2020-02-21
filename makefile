OSS       = oss
CHILD     = child
OSS_OBJ   = oss.o 
CHILD_OBJ = child.o
OUTPUT    = $(OSS) $(CHILD)
CC        = gcc
FLAGS     = -Wall -g

.SUFFIXES: .c .o

all: $(OUTPUT)

$(OSS): $(OSS_OBJ)
	$(CC) $(FLAGS) -o $@ $(OSS_OBJ)

$(CHILD): $(CHILD_OBJ)
	$(CC) $(FLAGS) -o $@ $(CHILD_OBJ)

.c.o:
	$(CC) $(FLAGS) -c $<

.PHONY: clean
clean:
	/bin/rm -f $(OUTPUT) *.o

