# Makefile for testing ATACS, by Eric Peskin
# Questions?  mailto:peskin@vlsigroup.ece.utah.edu
# Bug reports?  http://www.async.ece.utah.edu/cgi-bin/atacs

# This makefile relies heavily on features specific to GNU make.
# See info:make in the Gnome help browser or
# http://www.gnu.org/manual/make-3.79.1/html_node/make_toc.html

# This Makefile is run automatically by status.perl during a cvs commit.
# The full sequence is:
#   cvs consults  CVSROOT/commitinfo, 
#   which points to atacs/examples/regress
#   which  calls    atacs/examples/status.perl,
#   which  calls    atacs/examples/Makefile (via make)  <== You are here.

# However, you can also use it yourself, by hand.  You need to run it
# in the examples directory.  It will recurse into any sub-directories
# (except any named CVS).

# Whenever make completes, check the last line of output.  If any
# tests failed, the last line will report an Error.  If the last line
# of output from make does not report any error, then all the tests
# passed.

################# EXAMPLES OF HOW TO USE THIS MAKEFILE ########################

# To run the regression tests in your local directory, exactly as they
# would be run by status.perl at the pre-commit check:
#	make
# First this runs make in the atacs/src directory, to build the atacs
# executable, if necessary.  Then it tries those examples that have
# .gold files and only those examples.  For each such example, it
# checks that atacs can still match the expected results.

# To test the effect of different ATACS options on the regression
# tests, you can override any of the variables below (see the
# VARIABLES section) from the Unix shell command line.  For example:
#	make Extras=cP
# should check that ATACS passes the same regression tests even with
# -cP added to each invocation.

# To try to run ALL the examples (not just those with expected
# results) through ATACS, without comparing the results to anything,
# you can use the target PRSs.  WARNING: This tries ALL the examples.
# Some of these are huge.
#	make build PRSs

# > I'm trying to make all "unc" files using a different set of flags.
# > I do not want to compare against the gold's.  Is there a way to do
# > this?
# In your atacs/examples directory, try the following:
#       make Extras=${yourExtraFlags} unc/PRSs.subDir 

# To tell make to keep trying more examples, even if some examples
# fail, Use the --keep-going flag to make.  In this case, you will
# probably want to make it easier to see the errors coming from make.
# So redirect all the other output to some file
#	make --keep-going build PRSs > make.out # just redirect stdout
# Any example that fails will cause a message of the form 
# 	make[1]: *** [.<example>.prs${ATACSflags}] Error n
# A segmentation fault will show up as Error 139.
# If an example exceeds its cputime limit (see below) Error 152 will appear.
# To find more about what went wrong, you can look at ${Type}/<example>.log

# To find the examples that complete within just one second of cputime each:
#	make clean build                       # Build atacs executable.
#	limit cputime 1 second                 # Each process gets 1 second.
#	make --keep-going PRSs >& report1s     # Save stderr for next make.
#	find . -name ".*.prs*" > complete1s      # These examples completed.
#	grep 139$ report1s                     # These had segmentation faults.
#	grep 152$ report1s	               # These ran out of time.
#	egrep 'out of memory|storage' */*.log  # These ran out of memory.

# To find examples that did not complete above but do complete within a minute,
#  we can define the special variable Previous to point to our log from before:
#	limit cputime 1 minute                 # Each process gets 1 minute.
#	make --keep-going Previous=../report1s PRSs >& report1m
#	find . -name ".*.prs*" > complete1m
#	diff complete1s complete1m #These took between 1 second and 1 minute
# ... and so on ...

# To create a mirror of the relevant files (specification and expected results)
# from the examples directory:
#	make clean
#	make Mirror=/full/explicit/path/to/target/directory Certify

################# VARIABLES ###################################################
#ATACS switches (run atacs -hl for a list of the available switches):
bag	 =tb#  Use bourne again geometric timing to obtain state graphs.
bap	 =cPtp#  Use bourne again POSET timing to obtain state graphs.
baptdc	 =tt#  Use bourne again POSET timing with TDC to obtain state graphs.
geometric=tg#  Use geometric timing to obtain state graphs.
posets	 =ts#  Use Partial Order Sets timing to obtain state graphs.
hpn	 =#  Use HPN analyzer to obtain state graphs.
disabling=oD#  Allow rules to be disabled with only a warning.
insertion=oi#  Toggle automatic state-variable insertion.
noparg	 =oq#  Toggle use of PARG output on errors.
reduction=oR#  Toggle automatic concurrency reduction.
postproc =cP#  Toggle the use post-processing during compilation.
notfirst =PN#  Stop concurrency reduction after the first solution.
gatedelay=G#   Change default gate delay.
single	 =ys#  This command turns on the single cube algorithm for synthesis.
storenet =sn#  stores the final implementation in netlist format used by SYN
verify   =va#  Verify using ATACS.
hpnver   =tYllva#  Verify HPNs using ATACS.
vhdlams  =cPlvsgtYllva # Verify a VHDL-AMS file using ATACS

Extras   =#    user-specified options added to each atacs invocation
Timing   =${bap}${gatedelay}0-1#timing NOT used for burst-mode or Petri nets
DoIt     =${single}#  what to do
Special  =#    switches used by this Makefile to handle special cases

Root	 =..# root of the atacs tree  This will be modified as we recurse.
Kids     :=$(filter-out CVS/,$(wildcard */))# sub-directories to recurse into

atacs	 =${Root}/../bin/atacs#  location of atacs executable, unless...
ATACS	 :=$(firstword $(wildcard ${atacs})) # ...darn windows!

# path to the program to compare expected vs. actual results:
Diff     =${Root}/bin/compare_literals#  This must not contain spaces!
Difflags =-i#  options to be passed to above program --must not contain spaces!
checkHPN =${Root}/bin/checkHpn.pl
# ${Diff} ${Difflags} A B should return 0 if and only if A and B match.
# The above default value will only work for .eqn and .prs files.
# To handle any arbitrary file, set Diff=/usr/bin/diff

Previous =# path name to a file containing Error messages from a previous make
#  If Previous is defined, this Makefile will ONLY run atacs on examples
#  that failed in the previous make BECAUSE they ran out of cputime.
#  It determines this by looking for Error 152 in the file ${Previous}.
#  So the file that ${Previous} specifies must contain the stderr messages.

Mirror =# path name to target directory that is to become a mirror of
# all relevant files in the examples directory.  The value should not
# contain '~'.  Instead make it a full, explicit path.  If Mirror is
# defined, instead of actually making the targets, this makefile will
# copy the files it would have needed from the examples directory into
# the corresponding position underneath ${Mirror}.

Type     =$(notdir ${Dir})
ifneq ($(strip $(filter unc% g, ${Type})),)# burst-mode or Petri-net example
  override Timing  =# Do NOT use timing.
  ifneq (${Type},g)#  This must be some type of burst-mode example.
    Special = ob#       Use burst-mode technology option.
    ifeq (${Type},unc2l)#  This is a two-level Burst-mode example.
      Special = ol#        Use burst-mode AND gatelevel technology options.
      override Type = unc#   Expect .unc extension.
    endif
  endif
endif

Expected =gold#	       extension for files of expected results, e.g.,  gold
specFiles:=$(wildcard *.${Type}	  )#       specification files, e.g.,*.csp
goldFiles:=$(wildcard *.${Expected})# files of expected results, e.g.,*.gold
Targets  =$(basename  ${goldFiles})
Filter	 =${Targets}
ifdef PRSs
  Filter=$(addsuffix .prs, ${PRSs})
endif
Intersect=$(filter ${Filter}, ${Targets})
Sources  =${Targets:.prs=.${Type}} ${goldFiles}

dirt := $(wildcard dirt .cvsignore $(firstword $(wildcard ${Root}/examples/builtin /dev/null )))

Top	= top -d 1 -n 1
OS	:= $(shell uname -s)
free	= free
ifeq ($(OS),HP-UX)
  free = (set file=/tmp/${USER}.$$$$;${Top} -f $$file;cat $$file;${RM} $$file)
else
  ifneq ($(OS),Linux)
    free = ${Top}
  endif
endif
#memory	:= $(shell ${free} | awk '/Mem/ {print $$2}' | tr K k  2>&1)

version	:= $(shell bash -version -c "")
ifeq ($(strip $(filter 1.%, ${version})),)
  resource = v
else
  resource = d
endif

ATACSflags = -${Special}${Timing}${Extras}${DoIt}

#  Suffix for an empty file that indicates that we passed a test
#  The following assignment should cause this variable to change when any of
#  variables ${ATACSflags}, ${Mirror}, ${Diff}, or ${Difflags} change.
#  Do not change the following directly, unless you know what you're doing.
#  Instead, change the above variables.
OK := $(subst /,_,${ATACSflags}${Mirror}${Diff}${Difflags}).OK

### SUMMARY OF FILES, AS SEEN FROM THE atacs/examples DIRECTORY ###############
# Needed by this Makefile and managed by CVS:
# .cvsignore                         Filename patterns to match temporary files
# ${Type}/<example>.${Type}          Specification   , e.g., csp/ex1.csp
# ${Type}/<example>.prs.${Expected}  Expected results, e.g., csp/ex1.prs.gold
#
# Generated by atacs:
# ${Type}/<example>.prs                       Results, e.g., csp/ex1.prs
#
# Generated by this Makefile:
# ${Type}/.cvsignore        Generated from .cvsignore by removing "*.${Type}"
#
# ${Type}/.<example>.prs${OK} Time stamp, e.g., csp/.ex1.prs-mg_bin_diff.OK
#        This empty time-stamp is touched when and only when <example> passes 
#        a test.  It depends on the results and the expected results.
#        For example:        if csp/ex1.prs 
#                       matches csp/ex1.prs.gold
#                    =============================
#                    then touch csp/.ex1.prs-mg_bin_diff.OK
#
# ${Type}/<example>.log           messages from atacs, e.g., csp/ex1.log
##############################################################################

################# TARGETS AND RULES ###########################################
.DELETE_ON_ERROR:

all:	CVSignores build check verificationTests hpnTests vhdlamsTests # Build atacs, then run tests
	@echo
	@echo "CONGRATULATIONS -- ALL TESTS PASSED!  =) =) =)"

build:  # Build atacs from the source.
	${MAKE} -C ${Root}/src

#Certify:	all noPostProc hairy posets noX untimed channelTests paperChannelTests storeNetTests

Certify:	all hairy posets noX untimed channelTests paperChannelTests storeNetTests

hairy:
	${MAKE} Expected=night

noPostProc:
	${MAKE} Extras=${postproc}

hairyNoPostProc:
	${MAKE} Extras=${postproc} Expected=night

posets:
	${MAKE} Timing=${posets} Expected=posets

noX:
	${MAKE} Extras=${insertion} Timing=${geometric} Expected=noX

untimed:
	${MAKE} Extras=${insertion} Timing= Expected=untimed

reduce:
	${MAKE} Extras=${insertion}${reduction} Expected=reduce

reductionTests:
	${MAKE} untimedChannelTests reduce

verificationTests:
	${MAKE} DoIt=${verify} Expected=verify Timing=${posets}${gatedelay}0-1 verify

hpnTests:
	${MAKE} DoIt=${hpnver} Expected=hpnver Timing=${hpn} Type=lpn hpnver

vhdlamsTests:
	${MAKE} DoIt=${vhdlams} Expected=vhdlams Timing=${hpn} Type=vhd vhdlams

storeNetTests:
	${MAKE} ATACSflags=${single}${storenet} Expected=storenet verify

# Take any pseudo-target of the form <directory>/<target>.subDir to mean:
%.subDir:	# enter <directory>, and then make <target>
	${MAKE} -C $(@D) -f ${Root}/Makefile Root=../${Root} Dir=${Dir}/$(@D) $(*F)

#     Recursively, in each sub directory, can atacs match expected results?
check:	$(addsuffix check.subDir,${Kids}) \
	$(addprefix .,$(addsuffix ${OK}, ${Intersect}))

#     Recursively, in each sub directory, can atacs verify examples?
verify:	$(addsuffix verify.subDir,${Kids}) \
	$(addprefix .,$(addsuffix _VERIFY${ATACSflags}, ${Intersect}))

#     Recursively, in each sub directory, can atacs verify these hpn examples?
hpnver:	$(addsuffix hpnver.subDir,${Kids}) \
	$(addprefix .,$(addsuffix _HPNVER${ATACSflags}, ${Intersect}))

#  Recursively, in each sub directory, can atacs verify these vhdlams examples?
vhdlams: $(addsuffix vhdlams.subDir,${Kids}) \
	$(addprefix .,$(addsuffix _VHDLAMS${ATACSflags}, ${Intersect}))

#     Recursively, in each sub directory, run every example through atacs:
PRSs:	$(addsuffix  PRSs.subDir,${Kids}) \
	$(addprefix .,${specFiles:.${Type}=.prs${ATACSflags}})

#Build .cvsignore, recursively, in each sub-directory, and then...
clean:	CVSignores $(addsuffix clean.subDir,${Kids})
	cat ${dirt} | xargs --max-args 1 find -maxdepth 1 -type f -name | sort -u | xargs -r rm

CVSignores:	$(addsuffix .cvsignore,${Kids})

%/.cvsignore:	.cvsignore
	grep -vx "*.$*" $< > $@   #  Do not ignore spec files.

VHDLprs=${Component}.prs${ATACSflags}

.chan.${VHDLprs}:	Xchan.vhd Ychan.vhd chan.vhd
.wine2.${VHDLprs}:	winery.vhd  wine_shop.vhd patron.vhd  wine_example2.vhd
.wine3.${VHDLprs}:	winery.vhd  wine_shop.vhd patron.vhd  wine_example3.vhd
.wine224.${VHDLprs}:	winery2.vhd wine_shop.vhd patron2.vhd wine_example4.vhd
.wine324.${VHDLprs}:	winery3.vhd wine_shop.vhd patron2.vhd wine_example4.vhd
.wine424.${VHDLprs}:	winery4.vhd wine_shop.vhd patron2.vhd wine_example4.vhd
.wine524.${VHDLprs}:	winery5.vhd wine_shop.vhd patron2.vhd wine_example4.vhd
.wine934.${VHDLprs}:	winery9.vhd wine_shop.vhd patron3.vhd wine_example4.vhd
.mergex.${VHDLprs}:	winery2.vhd     merge.vhd patron.vhd    mergexample.vhd
.newMergex.${VHDLprs}:	winery2.vhd  newMerge.vhd patron.vhd newMergexample.vhd
.convey.${VHDLprs}:     producer.vhd  consumer.vhd conveyor.vhd
.PP.${VHDLprs}:         winery.vhd wine_shopPP.vhd patron.vhd PPtop.vhd
.PA.${VHDLprs}:         winery.vhd wine_shopPA.vhd patron.vhd PAtop.vhd
.AP.${VHDLprs}:         winery.vhd wine_shopAP.vhd patron.vhd APtop.vhd
.AA.${VHDLprs}:         winery.vhd wine_shopAA.vhd patron.vhd AAtop.vhd
.MPEG.${VHDLprs}:       cnt.vhd mem.vhd MTT.vhd dpCal.vhd datapath_ctrl.vhd datapath.vhd MpegDecoder.vhd mpegdecoder_TB.vhd

ifdef Component
  Base	= ${Component}
else
  Base	= $*
  Spec	= %.${Type}
endif

# Run a given example through atacs, and compare actual with expected results
.%.prs${ATACSflags}:	${ATACS}  ${Spec}
      ifdef Previous     # Only try example if it ran out of cputime last time.
	grep -q "\[$@\] Error 152" ${Previous}
      endif #Otherwise, the above grep will fail, and this rule will stop here.
      ifdef Mirror
	mkdir -p ${Mirror}${Dir}
	cp -pf $(filter-out ${ATACS},$^) ${Mirror}${Dir}
	touch $@
      else
	${RM} ${Base}.prs #  Remove stale results.
#	ulimit -S${resource} ${memory} -St 3600;
	$^ ${Component} ${ATACSflags} > ${Base}.log 2>&1
	cp ${Base}.prs $@
      endif

.%.prs${OK}:	${Diff} .%.prs${ATACSflags} %.prs.${Expected}
      ifdef Mirror
	cp -pf $*.prs.${Expected} ${Mirror}${Dir}
      else
	$< ${Difflags} .$*.prs${ATACSflags} $*.prs.${Expected}
      endif
	touch $@
	@echo

.%_VERIFY${ATACSflags}:	${ATACS} ${Spec}
	$^ ${Component} ${ATACSflags} > ${Base}.log 2>&1
	touch $@
	@echo

.%_HPNVER${ATACSflags}:	${ATACS} ${Spec}
	$^ ${ATACSflags} > ${Base}.log 2>&1
	${checkHPN} ${Base}
	touch $@
	@echo

.%_VHDLAMS${ATACSflags}: ${ATACS} ${Spec}
	$^ ${ATACSflags} > ${Base}.log 2>&1
	${checkHPN} ${Base}
	touch $@
	@echo

%.VHDLtest:
	${MAKE} -f ${Root}/examples/Makefile Expected=VHDLgold Component=${*F} .${@D}.${*F}.prs${OK}


VHDLtests:$(addsuffix .VHDLtest, chan/Xchan wine3/NEW_SHOP wine224/THE_SHOP wine324/THE_SHOP wine424/THE_SHOP wine524/THE_SHOP wine934/THE_SHOP PP/PP PA/PA AP/AP AA/AA mergex/merge newMergex/newMerge)

channelTests:
	${MAKE} Special=${insertion}${noparg} Timing=${bag}${gatedelay}1-2 vhd/VHDLtests.subDir

paperVHDLtests:$(addsuffix .VHDLtest, MPEG/mpegdecoder.ctrl)

paperChannelTests:
	${MAKE} Special=${insertion}${noparg}${notfirst} Timing=${disabling}${bap}${gatedelay}1-2 vhd/paperVHDLtests.subDir

untimedChannelTests:
	${MAKE} Special=${insertion}${noparg} Timing= vhd/VHDLtests.subDir

%.test:
	${MAKE} ATACSflags=$(suffix $*) $*.subDir
