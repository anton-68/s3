# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
CC		    := g++
LD			:= g++
INCLUDE		:= -I/usr/local/include
CFLAGS		:= -g -Wall -DPTRACING=1 -D_REENTRANT -fno-exceptions -fPIC #-std=gnu++0x 
LFLAGS		:= -DPTRACING=1 -D_REENTRANT -fno-exceptions $(INCLUDE)
LIBDIR		:= -L$(LD_LIBRARY_PATH)
LIBS		:= -ldl -lpt -lopal -lc
LIBS_D		:= -ldl -lpt_d -lopal_d -lc
OBJECTS		:= $(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY					:	all
all						:	s3

%.o						: 	%.cpp %.h
							$(CC) $(CFLAGS) -c $(subst .o,.cpp,$@) $(INCLUDE)/ptlib $(INCLUDE)/opal

s3						:	$(OBJECTS)
							$(LD) $(LFLAGS) -o s3 $(OBJECTS) $(LIBDIR) $(LIBS)

.PHONY					:	clean
clean:
							rm -f *.o *~ s3 s3_d *.log

.PHONY					:	rebuild
rebuild:					clean all
