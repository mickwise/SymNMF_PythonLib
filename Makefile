CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
TARGET = symnmf
SOURCES = symnmf.c symnmf_ops.c memory_management.c
HEADERS = symnmf.h symnmf_ops.h memory_management.h
OBJECT_FILES = symnmf.o symnmf_ops.o memory_management.o

all:	$(TARGET)

$(TARGET):	$(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $@ $(OBJECT_FILES) -lm

%.o:	%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECT_FILES) $(TARGET)