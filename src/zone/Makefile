### compiler
CC           	= g++
LEX		= flex
YACC		= bison

### Include directories to search for #include<>'s
INCLUDES        = 

### Library directories and names for the linker
LIBDIR		= 
RELEASELIB	= 

### Name of the library
LIBNAME         = 

### Compiler flags
CFLAGS		= -O3 -lm

### Defines for preprocessor
DEFINES         = 

LINK		= $(CC) $(DEFINES) $(INCLUDES) $(CFLAGS) $(OBJECTS) \
		  $(LIBDIR) $(LIBNAME) -o zone 

PURIFY 		= purify -collector=/usr/local/lib/gcc-lib/hppa1.1-hp-hpux10.20/2.8.1/ld
#===================================================================

PGMS		= zone

OBJECTS	        = zone_test.o zone.o 

all:		$(PGMS)

# ------------------------------------------------------------------

%.o:	%.cpp
	$(CC) $(DEFINES) $(INCLUDES) $(CFLAGS) -c $<

zone:	$(OBJECTS) zone2ps.pl
	$(LINK)
	cp -pf zone2ps.pl ../../bin/.

purify: $(OBJECTS)
	$(PURIFY) $(LINK) 

# ------------------------------------------------------------------

clean:
		rm -f *.o $(PGMS) core *.dxx
