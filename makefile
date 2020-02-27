OSS       = oss
OSS_OBJ   = oss.o ossOptions.o $(SHARED_O)
OSS_H     = ossOptions.h $(SHARED_H)

CHILD     = child
CHILD_OBJ = child.o $(SHARED_O)
CHILD_H   = $(SHARED_H)

SHARED_H  = sharedMemory.h perrorExit.h shmkey.h clock.h
SHARED_O  = sharedMemory.o perrorExit.o clock.o

OUTPUT    = $(OSS) $(CHILD)
CC        = gcc
FLAGS     = -Wall -g

.SUFFIXES: .c .o

all: $(OUTPUT)

$(OSS): $(OSS_OBJ) $(OSS_H)
	$(CC) $(FLAGS) -o $@ $(OSS_OBJ)

$(CHILD): $(CHILD_OBJ) $(CHILD_H)
	$(CC) $(FLAGS) -o $@ $(CHILD_OBJ)

.c.o:
	$(CC) $(FLAGS) -c $<

.PHONY: clean
clean:
	/bin/rm -f $(OUTPUT) *.o 

