#
# Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
#
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

AR	=ar
CC	=gcc
TARGET	=libtrace.a
CFLAGS	=-Wall
OBJ	=tool.o trace.o


.SUFFIXES: .c .o .h


# primary target
.PHONY: all
all: depend $(TARGET)


# main target
$(TARGET): $(OBJ)
	$(AR) r $@ $^


# Cleaning
.PHONY: clean cleanup
clean:
	-@$(RM) $(OBJ)
	-@$(RM) depend.inc

cleanup: clean
	-@$(RM) $(TARGET)


# Suffixes for .o (.c -> .o)
.c.o:
	$(CC) $(CFLAGS) -c $<


# header file dependency calculation
.PHONY: depend
depend: $(OBJ:.o=.c)
	-@$(RM) depend.inc
	-@for i in $^; do\
		$(CC) $(CFLAGS) -MM $$i | sed "s/\ [_a-zA-Z0-9][_a-zA-Z0-9]*\.c//g" >> depend.inc;\
	done

-include depend.inc
