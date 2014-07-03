#
# Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
#
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

CC	=gcc
TARGET	=chatserv
CFLAGS	=-I../lib -Wall
OBJ	=main.o lisn.o conn.o
LDFLAGS	=-L../lib
LIBS	=-ltrace


.SUFFIXES: .c .o .h


# primary target
.PHONY: all debug
all: depend $(TARGET)

# debug target
debug: DB_CFLAGS =-g
debug: all


# main target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)


# Cleaning
.PHONY: clean cleanup
clean:
	-@$(RM) $(OBJ)
	-@$(RM) depend.inc

cleanup: clean
	-@$(RM) $(TARGET)


# Suffixes for .o (.c -> .o)
.c.o:
	$(CC) $(CFLAGS) $(DB_CFLAGS) -c $<


# header file dependency calculation
.PHONY: depend
depend: $(OBJ:.o=.c)
	-@$(RM) depend.inc
	-@for i in $^; do\
		$(CC) $(CFLAGS) -MM $$i | sed "s/\ [_a-zA-Z0-9][_a-zA-Z0-9]*\.c//g" >> depend.inc;\
	done

-include depend.inc
