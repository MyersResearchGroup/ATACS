.DELETE_ON_ERROR:

# Location of C++ Compiler, lex, and yacc.
CC = g++
##
## Enable this line and run on sanguo if you want run insure to check
## for memory leaks in your code.  Don't forget to comment out the
## other CC line.
## NOTE: You must added /usr/local/src/insure/bin.linux to your path for
## this to work correctly. The easiest way to do this with tcsh is
## set path = { $path /usr/local/... }
#CC = insure -g -DDDEBUG

VPATH = /usr/bin

LEX = flex -i
YACC = bison

OS := $(shell uname -s)
ARCH := $(shell uname -m)
RELEASE := $(shell uname -r)

BASIC_DEFS = -std=c++11 -DTIMERS -DEXPANDNET -DMEMSTATS # -DXIF -DDDEBUG -D_NO_STATS_ -DMATSTATS -DSUBTIME

BIN	= ../bin

#TCLINC = -I/usr/include/tcl
TCLINC = 
PKGINC = -I../../include
MDBINC = -I/home/ming/software/src/memdebug
INC = $(TCLINC) $(PKGINC)
YFLAGS = -vd
XFLAGS1 = -DX -I/usr/X11R6/include 
XFLAGS2 = -L/usr/X11R6/lib -lX11 

MDBDEF = -DMEMDEBUG

ifeq ($(OS),Linux)
  ifeq ($(ARCH),ppc)
    PKGINC = -I/home/software/include
    PKGLIBS = -L/home/software/lib
    MDBINC = -I/home/software/include
    MDBLIB = ${PKGLIBS} -L/home/ming/software/src/memdebug -lmemdb
    XLIB = -L/usr/X11R6/lib
#    TCLTKLIBS = -ltk -ltcl -ldl
    TCLTKLIBS =
    STATLIBS =
    DEFS = $(BASIC_DEFS) -D_NO_STATS_
  else
    ifeq ($(ARCH),x86_64)
      MDBLIB = -L/home/ming/software/src/memdebug -lmemdb
      PKGLIBS = -L../../lib64
      XLIB = -L/usr/X11R6/lib
#    TCLTKLIBS = -ltk -ltcl -ldl
      TCLTKLIBS = 
      STATLIBS = -lprob
      DEFS = $(BASIC_DEFS)
    else
      MDBLIB = -L/home/ming/software/src/memdebug -lmemdb
      PKGLIBS = -L../../lib
      XLIB = -L/usr/X11R6/lib
#    TCLTKLIBS = -ltk -ltcl -ldl
      TCLTKLIBS = 
      STATLIBS = -lprob
      DEFS = $(BASIC_DEFS)
    endif
  endif
  PKGINC += -I/usr/src/linux/include
  REDHAT_RELEASE := $(shell cat $(wildcard /etc/redhat-release /dev/null))
  ifneq ($(strip $(filter 6.%,${REDHAT_RELEASE})),)
    PKGINC += -I.
  endif
  ifneq ($(strip $(filter 8.%,${REDHAT_RELEASE})),)
    #  To compile ATACS on RedHat 8 until we figure out GCC version 3.2:
    #    install:
    #      compat-gcc-7.3-2.96.110
    #      compat-gcc-c++-7.3-2.96.110
    #      compat-libstdc++-7.3-2.96.110
    #      compat-libstdc++-devel-7.3-2.96.110
    #    and set:
    CC = g++
  endif
else
  ifeq ($(OS),SunOS)
    MDBLIB = -L/home/yuan/myers/memdebug -lmemdb
    PKGLIBS = -L/home/ching/atacs/lib
    XLIB =
#    TCLTKLIBS = /usr/local/lib/libtk4.0.a /usr/local/lib/libtcl7.4.a
    TCLTKLIBS = 
    STATLIBS =
    DEFS = $(BASIC_DEFS) -D__TEMPLATE_PATCH -D_NO_STATS_
  else
    ifeq ($(OS),HP-UX)
      MDBLIB = -L/home/yuan/atacs/memdebug -lmemdb
      PKGLIBS = -L/home/yuan/atacs/lib
      XLIB = -L/lib/X11R5
#      TCLTKLIBS = /usr/local/lib/libtk4.0.a /usr/local/lib/libtcl7.4.a
      TCLTKLIBS =
      STATLIBS =
      DEFS = $(BASIC_DEFS) -D__TEMPLATE_PATCH -D__HPUX -D_NO_STATS_
    else
ifeq ($(HOSTNAME),IRONMAN)
      CC = g++-2
      DEFS = $(BASIC_DEFS) -DNT
#      TCLINC = -I/usr/X11R6/include -I/usr/include
      PKGINC = -I/home/curt/research/tools/winlib/include
      INC = $(TCLINC) $(PKGINC)
      YFLAGS	= -vd
      XFLAGS1 = -DX 
      XFLAGS2 = -L/home/curt/research/tools/winlib/lib -lX11
      XLIB = -L/usr/X11R6/lib
#      TCLTKLIBS =  -ltk80 -ltcl80 
      TCLTKLIBS = 
      PKGLIBS = -L/home/curt/research/tools/winlib/lib 
      BASIC_DEFS = -DXIF -DTIMER -D_NO_STATS_
else
ifeq ($(OS), Darwin)
#      TCLINC = -I/sw/include
      #TCLTKLIBS = -L/sw/lib
      PKGINC = -I/usr/local/include
      PKGLIBS = -L/usr/local/lib -L/sw/lib
      XLIB = -L/usr/X11R6/lib
#      TCLTKLIBS = -ltcl -ltk -ldl
      TCLTKLIBS = 
      STATLIBS = -lprob
#      STATLIBS = -lepd -lprob
      #DEFS = $(BASIC_DEFS)
    #MDBINC = -I/home/software/include
    #MDBLIB = ${PKGLIBS} -L/home/ming/software/src/memdebug -lmemdb
      DEFS = $(BASIC_DEFS) -D_NO_STATS_ -DOSX 
else
      #echo "Assuming MSWindows OS"
      DEFS = $(BASIC_DEFS) -DNT
#      TCLINC = -I/cygdrive/e/research/nobackup/cad/cygwin/cygwin-b20/include
      PKGINC = -I../../include
      INC = $(TCLINC) $(PKGINC) #-I/cygdrive/e/research/nobackup/cad/cygwin/cygwin-b20/H-i586-cygwin32/i586-cygwin32/include
      YFLAGS	= -vd
      XFLAGS1 = -DX 
      XFLAGS2 = -L../../bin
      XLIB = #-L/cygdrive/e/research/nobackup/cad/cygwin/cygwin-b20/H-i586-cygwin32/lib
#      TCLTKLIBS = -L/cygdrive/e/research/nobackup/cad/cygwin/cygwin-b20/H-i586-cygwin32/lib -ltk80 -ltcl80 
      TCLTKLIBS = 
      PKGLIBS = -L../../winlib -L../../bin
      BASIC_DEFS = -DTIMER -D_NO_STATS_
    endif
  endif
endif
endif
endif

ifeq ($(HOSTNAME),jiaqi)
  PKGINC = -I/usr/local/include -I/usr/src/linux/include
  MDBINC =
  MDBLIB = -lmemdb
  PKGLIBS = -L/usr/local/lib
endif

ifeq ($(HOSTNAME),zorac.elen.utah.edu)
  PKGINC = -I/usr/local/include
  PKGLIBS = -L/usr/local/lib
endif

ifeq ($(HOSTNAME),wu.elen.utah.edu)
  PKGINC = -I/usr/local/include
  PKGLIBS = -L/usr/local/lib
endif

ifeq ($(HOSTNAME),mudd.cs.byu.edu)
  PKGINC = -I/usr/local/include
  PKGLIBS = -L/usr/local/lib
endif

ifeq ($(HOSTNAME),liao)
  PKGINC = -I/usr/local/include
  PKGLIBS = -L/usr/local/lib
endif

#ifeq ($(HOSTNAME),tang.ece.utah.edu)
#  PKGINC = -I/home/tang/myers/BioSim/include
#  PKGLIBS = -L/home/tang/myers/BioSim/lib64
#endif

# To add a new source file, just create the source file, cvs add it,
# and add its name to the variable SOURCES.  The Makefile will
# automatically treat the corresponding .o file as a target, and scan
# the #include statements for prerequisites
SOURCES = atacs.c misclib.c addord.c storeg.c loadsis.c loader.c	\
struct.cpp loadprs.c findtd.c findred.c connect.c addpers.c findrsg.c	\
findreg.c findconf.c findcover.c storeprs.c storesg.c memaloc.c		\
printgrf.c printrsg.c printreg.c tersgen.c compile.c help.c breakup.c	\
findwctd.c findcl.c delaycalc.c simulate.c hse.cpp orbitsrsg.c		\
verify.c struct2net.c storenet.c storeer.c resolve.c exact.c loadrsg.c	\
hhash.c hmisc.c hxbm2csg.c merges.c actions.c events.c rules.c		\
conflicts.c makedecl.c hash.c bddrsg.c symtab.c types.c auxilary.c	\
drawnet.c geom.c common_timing.c approx.c storevhd.c dotrsg.c dotreg.c	\
dotgrf.c loadg.c findstats.c project.c CPdf.cpp CNormal.cpp		\
CUniform.cpp CSpline.cpp CUdef.cpp markov_matrix_func.cpp		\
markov_periodic_analysis.cpp markov_chain_analysis.cpp			\
rg_trans_prob.cpp simtel.c stateasgn.c graph_tracker.cpp		\
cpu_time_tracker.cpp CSingular.cpp signal_handler.cpp CTextTel.cpp	\
cudd_synth.c trigger_probs.cpp markov_exp.cpp bm_trans_probs.cpp	\
partition.cpp cycper.cpp printsg.c svhelpers.cpp			\
markov_matrix_check.cpp lt_tp_tracker.cpp tels_table.cc heursyn.c	\
trgtrk.cpp create_env.cc classes.cc bcp.c pbsuuaad.cpp kiss2sg.c	\
stats.c postproc.cc error.cc telwrapper.cc untimed_state.cc		\
timed_state.cc POSET.cc timed_state_space.cc bag.cc bap.cc tel2vhd.c	\
po_reduce.cc level_expand.cc decomp.c statecode.cpp svclasses.cpp 	\
loadac.c printlpn.c lpnrsg.c writeacc.c haz_timed.c haz_untim.c         \
srllpnrsg.c techmap_print.c match_cov.c techmap_misc.c lpntrsfm.c       \
color.c loadlib.c hpnsim.c hpnrsg.c displayADT.c symbolic.c satconf.c   \
partial_order.cpp lhpnrsg.c ctlchecker.c biomap.c biostructure.c biostructure_test.c \
test.c hash_map_test.c lhpnsim.c loadData.c smtsym.c \

OBJECTS = $(addsuffix .o, $(basename ${SOURCES}) gparse ctlparse tctlparse lex.yy parser.tab gateParse interactParse)
AllObjs = $(OBJECTS) solvecsc.o slack.o

#CFLAGS = $(DEFS) $(MDBDEF) $(INC) $(MDBINC) -Wall -g
CFLAGS = $(DEFS) $(INC) -Wall -g #-Wno-deprecated #-static
Compile = $(CC) $(CFLAGS) $(BFLAG) -c#  Compile a source file.
Command = ${Compile} $<# Compile the source file that is the first prerequisite
liBase = $(PKGLIBS) $(XLIB) $(TCLTKLIBS) $(STATLIBS) -lm 
LIBS = ${liBase} -lBdd -lutil -lbdd -lmem

#change the CLIBS for machines using cudd 2.3.1 and >
CLIBS = ${liBase} #-lobj -lcudd -lCutil -lmtr -lst -lepd -lyices
ifeq ($(HOSTNAME),localhost.localdomain)
 CLIBS = ${liBase} -lobj -lcudd -lCutil -lmtr -lst -lepd -lyices
endif
ifeq ($(HOSTNAME),liao)
 CLIBS = ${liBase} -lobj -lcudd -lCutil -lmtr -lst -lepd -lyices
endif

ifeq ($(HOSTNAME),mudd.cs.byu.edu)
 CLIBS = ${liBase} -lobj -lcudd -lCutil -lmtr -lst -lepd -lyices
endif

Link = $(CC) $(CFLAGS) $^ $(CLIBS) $(TSE) -o ${BIN}/atacs#Link object files.
Purify = purify -collector=/usr/local/lib/gcc-lib/hppa1.1-hp-hpux10.20/2.8.1/ld

# This is used to build with ccmalloc.o  ccmalloc is a memory analysis tool
# that can detect memory leaks, undewrites, overwrites, etc.  The source
# file can be found in ~eemercer/src/ccmalloc-0.2.3.tar.gz -- 05/23/01
CCMALLOC_LINK =	ccmalloc ${Link}

all:	${BIN}/atacs

opt:	
	${MAKE} parser.tab.o 
	${MAKE} CFLAGS='$(DEFS) $(INC) -Wall -O3'

static:
	${MAKE} CFLAGS='-DTIMER -DMEMSTATS $(INC) -Wall -O3 -static' \
		CLIBS='$(PKGLIBS) $(XLIB) $(STATLIBS) -lm -lobj \
		-lcudd -lCutil -lmtr -lst -lepd' 

prof:	
	${MAKE} CFLAGS='$(DEFS) $(INC) -Wall -O3 -pg'

clean:
	${RM} $(shell cat .cvsignore)
#	${RM} $(addprefix ${BIN}/,$(shell cat ${BIN}/.cvsignore))

lex.yy.c: parser.l
	$(LEX) parser.l

parser.tab.h parser.tab.c: parser.y
	$(YACC) $(YFLAGS) parser.y

glex.c: glex.l
	$(LEX) -Pg -t glex.l > glex.c

gparse.c: glex.c gparse.y
	$(YACC) $(YFLAGS) -p g gparse.y -o gparse.c

ctllex.c: ctl.l
	$(LEX) -Pctl -t ctl.l > ctllex.c

ctlparse.c: ctllex.c ctl.y
	$(YACC) $(YFLAGS) -p ctl ctl.y -o ctlparse.c

tctllex.c: tctl.l
	$(LEX) -Ptctl -t tctl.l > tctllex.c

tctlparse.h tctlparse.c: tctllex.c tctl.y
	$(YACC) $(YFLAGS) -p tctl tctl.y -o tctlparse.c

gateConstruction.c: gateConstruction.l
	$(LEX) -t -Pgate gateConstruction.l > gateConstruction.c

gateParse.c: gateConstruction.c gateParse.y
	$(YACC) $(YFLAGS) -p gate gateParse.y -o gateParse.c

interaction.c: interaction.l
	$(LEX) -t -Pinteract interaction.l > interaction.c

interactParse.c: interaction.c interactParse.y
	$(YACC) $(YFLAGS) -p interact interactParse.y -o interactParse.c

%.o: %.c;  ${Command}
%.o: %.cc;  ${Command}
%.o: %.cpp;  ${Command}

windows: $(OBJECTS) 
	mkdir -p ${BIN}
	$(CC) $(CFLAGS) $^ $(CLIBS) $(TSE) -o ${BIN}/atacs
#	cp -pf $^.exe ${BIN}
#	ln -fs atacs.exe ${BIN}/xxatacs.exe

${BIN}/atacs: $(OBJECTS)
	mkdir -p ${@D}
	${Link}
#	ln -fs atacs ${@D}/xxatacs
	${RM} ${@F} #  Use the version in ${@D} instead.

mdb: ${OBJECTS};  ${Link} ${MDBLIB}
purify: ${OBJECTS};  ${Purify} ${Link}

# Includes ccmalloc.o in the link using gcc. Note, this explicitly
# includes libstdc++ for gcc.
ccmalloc: $(OBJECTS);  ${CCMALLOC_LINK}

dlong:
	${MAKE} BFLAG="-DDLONG" CLIBS="${LIBS}"

intel: ${AllObjs}
	${Compile} -DINTEL findrsg.c atacs.c help.c
	${Link} -DINTEL

parg: parg.c layout.o
	gcc -g $(XFLAGS1) -o $@ parg.c layout.o -lm $(XFLAGS2)
	mv parg ../bin/parg

layout.o: layout.c
	gcc -c layout.c

# This section automatically generates prerequisites, using the strategy from
# http://www.gnu.org/manual/make-3.79.1/html_node/make_44.html#SEC43 or
# info:make#Automatic_Prerequisites

#  The practice we recommend for automatic prerequisite generation is
#  to have one makefile corresponding to each source file. For each
#  source file `name.c' there is a makefile `name.d' which lists what
#  files the object file `name.o' depends on. That way only the source
#  files that have changed need to be rescanned to produce the new
#  prerequisites.

#  Here is the pattern rule to generate a file of prerequisites (i.e., a
#  makefile) called `name.d' from a C source file called `name.c':

Depend = ${Command} -M | ${BIN}/prereqs.perl > $@

%.d: %.c;  ${Depend}
%.d: %.cc;  ${Depend}
%.d: %.cpp;  ${Depend}

#  The purpose of the sed command is to translate (for example): 
#    main.o : main.c defs.h
#  into: 
#    main.o main.d TAGS: main.c defs.h

#  This makes each `.d' file (and the emacs TAGS table) depend on all
#  the source and header files that the corresponding `.o' file
#  depends on. make then knows it must regenerate the prerequisites
#  whenever any of the source or header files changes.

#  Once you've defined the rule to remake the `.d' files, you then use
#  the include directive to read them all in. See
#  http://www.gnu.org/manual/make-3.79.1/html_node/make_16.html#SEC15 or
#  info:make#Include
#  For example:

ifneq ($(MAKECMDGOALS),clean)
-include $(AllObjs:.o=.d)# Read all the .d files, remaking them if necessary.
endif

# Build an emacs tags table.  Note that the automatic prerequisite
# generation above should add all the sources that are not explicitly
# listed here as prerequisites:
TAGS:  etags parser.l parser.y glex.l gparse.y gateConstruction.l gateConstruction.y interaction.l interactParse.y ctl.l ctl.y tctl.l tctl.y
	$^
# To learn what you can do with a tags table, see the emacs documentation.
# For example, see info:emacs#Tags in the gnome help browser.
# Also, from within emacs itself, try Help->Describe->Apropos Commands
# and type "tags"
# It's way cool...
