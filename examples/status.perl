#eval "set PATH=/usr/bin:/usr/local/bin:/Perl/bin:${PATH};exec perl -S $0 $*"
eval "set PATH=/usr/bin:/usr/local/bin:/Perl/bin:${PATH};exec perl -S $0"
    if $running_under_some_shell;#above line won't run if we're already in Perl

use English; # Allow the long names for perl's built-in variables.

# Script to predict the impact of a local cvs commit
# Copyright (C) 1999-2002, Eric Robert Peskin
# Questions?  mailto:peskin@vlsigroup.ece.utah.edu
# Bug reports?  http://www.async.ece.utah.edu/cgi-bin/atacs

# regress calls this script exactly once during each cvs commit.
# It calls it from the directory from which cvs was called.
# It passes the same file arguments that were passed to cvs.

# The full sequence is:
#   cvs consults CVSROOT/commitinfo, 
#   which points to /home/ming/remote/atacs/examples/regress,
#   which calls your local atacs/examples/status.perl,  <== You are here.
#   which calls your local atacs/examples/Makefile (via make)

# However, you can also use it yourself, by hand.
# Run this script from the directory where you would run cvs commit.
# Pass this script the same file arguments you would pass to cvs commit,
# but do NOT include any of the options:

# Synopsis: status.perl [files...]

# As with cvs commit, if no files are given, this script assumes you
# intend to recursively commit everything from the current working
# directory downward.

# The job of this script is to warn the user if the corresponding cvs
# commit would be likely to break the repository.  In particular, it
# checks to see whether the user has modified other files that are not
# being committed.

# If it issues any such warnings the user is given the option to
# abort.  Otherwise, this script calls make on the
# atacs/examples/Makefile to run the regression tests.
# If anything fails, this script exits with a non-zero status.

$OUTPUT_RECORD_SEPARATOR =  "\n";  # appended to each print

chomp($myDir   =`pwd`);  # Directory from which CVS was (or would be) called
$myDir =~ /^(.*\/atacs)((\/.*)?)$/ or die "Could not find root of atacs tree";
chdir($ATACSdir= $1  )             or die "Could not cd to $ATACSdir"        ;
print                   "Checking status of tree rooted at $ATACSdir/"	     ;
$pattern = join('|',@ARGV); # pattern will match any file given on command line
$problems=  0             ; # the number of problems encountered so far

#Check status of local tree with respect to the repository, by parsing output
#from a cvs update command.
# -f means ignore user's ~/.cvsrc file, using only the flags I specify.
# -n means do NOT actually update anything.  Just produce output.
# -q suppresses silly messages about recursion.
# -A resets any sticky tags.
# -P ignores any sub-directories that would be empty except for CVS management.
#See http://www.gnu.org/manual/cvs/html_node/cvs_87.html#SEC89 for -fnq.
#See http://www.gnu.org/manual/cvs/html_node/cvs_126.html#SEC133 for -AP.
#See http://www.gnu.org/manual/cvs/html_node/cvs_127.html#SEC134 for the output
open (REPORT,"cvs -fnq update -AP|") or die "Could not start cvs: $OS_ERROR";
 line: while (<REPORT>) {
     chomp;                         # Kill any trailing carriage return.
     if(/^\S\s+/){                  # Message of the form "X filename"
	 $file = $POSTMATCH;        # but what is X?...
	 if   (/^C/   ){
	     print  "WARNING $ARG: Your changes CONFLICT with someone else's.";
	     $problems++;
	 }
	 elsif(/^\?/  ){
	     print  "WARNING $ARG: You have not cvs added this file.";
	     $problems++;
	 }
	 elsif(/^[UP]/){
	     print  "WARNING $ARG: Another user has modified this file.";
	     $problems++;
	 }
	 elsif("$ATACSdir/$file" !~ /^$myDir\/($pattern)/){#not committing file
	     printf "WARNING $ARG: You have "			;
	     printf    "add" if	 /^A/				;
	     printf  "remov" if	 /^R/				;
	     printf "modifi" if	 /^M/				;
	     print	   "ed this file but not committed it." ;
	     $problems++					;
	 }
	 $file =~ s/\.[^\/]*$//              ;# delete any filename extensions
	 system("rm -f $file.{prs,PRS,*.ok}");#clean up example's stale results
     }
 }
close(REPORT             ) or die "cvs returned $CHILD_ERROR" ;
if($problems){
    print  "Committing just @ARGV could break the repository!" if @ARGV;
    printf "Are you sure you want to proceed (yes/no)?  "              ;
    (<STDIN>=~ /^\s*yes/i) or die "Aborted by user"                    ;
}
open (LOG ,">.status.log") or die "could not open  .status.log";
open (MAKE,"make -C examples 2>&1|") or die "cannot start make: $OS_ERROR";
 line: while (<MAKE>) {
     printf    ;  #  Copy the output from make to stdout...
     printf LOG;  #  ... and to a log file.
 }
close(MAKE               ) or die "make failed in examples :  $CHILD_ERROR";
close(LOG                ) or die "could not close .status.log";
system("touch .status.ok;  chgrp -Rf atacs .;  chmod -Rf g+rX .; chmod -f o+rX `find -type d -not -path \"*/CVS*\"`") ;  # Success!
