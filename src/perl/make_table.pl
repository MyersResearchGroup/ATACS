eval "set PATH=/usr/bin:/usr/local/bin:/Perl/bin:${PATH};exec perl -S $0 $*"
    if $running_under_some_shell;#above line won't run if we're already in Perl

##########################################################################
## egm -- 12/18/00
##
## This perl script will take 2 files that contain on each line the 
## following information:
##
## <filename> #zones #states #max_stack_depth #max_mem_usage #cpu_time
##
## The script makes the following assumptions that must be garaunteed by
## the user:
##
## 1) Both files are the same size;
## 2) <filename> at given line number of each file is the same. (i.e., 
##    both files have the sames entries in the same place );
## 3) The method used to generate the data file is the name of file.
## The script produces a LaTex table that with the data from each file
## side by side ( i.e., FILE1 DATA : FILE2DATA ).
##
##########################################################################

# Make sure make_table.pl is called with 2 files on the command line
if ( $#ARGV != 1 ) { die "usage: make_table.pl file1 file2 \n"; }

# Check that each file can be opened and read
if ( ! -r @ARGV[0] ) { die "ERROR: could read ".@ARGV[0]."" }
if ( ! -r @ARGV[1] ) { die "ERROR: could read ".@ARGV[1]."" }

## Get the base name for each file.
($file1_basename = @ARGV[0]) =~ s#.*/##; # Strips leading information
$file1_basename =~  s/\..*//;            # Removes extension
$file1_basename =~ s#_+#\\_#g;           # Correctly handles the '_' char
($file2_basename = @ARGV[1]) =~ s#.*/##;
$file2_basename =~  s/\..*//;
$file2_basename =~ s#_+#\\_#g;           # Correctly handles the '_' char

# Print the header information for the LaTEX file and table.
print "\\documentclass{article}\n";
print "\\usepackage{longtable}\n";
print "\\usepackage{color}\n";
print "\\textwidth 6.7in \n";
print "\\textheight 9in \n";
print "\\topmargin -0.5in \n";
print "\\oddsidemargin -0.2in \n";
print "\\begin{document}\n";
print "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
print "%%                       BEGIN TABLE                      %%\n";
#print "\\begin{table}\n";
#print "\\caption{Comparision of 2 timed state space exploration methods.}\n";
#print "\\vspace*{0.1in}\n";
print "\\begin{center}\n";
print "{\\catcode`|=12 \\def\\c#1{\\multicolumn{1}{c|}{#1}}\n";
print "\\def\\cc#1{\\multicolumn{1}{c||}{#1}} \\def\\dnc#1";
print "{\\multicolumn{2}{c|}{#1}}\n";
print "\\begin{longtable}{|l||r|r|r|r|r||r|r|r|r|r|}\n";
print "\\hline \n";
print " & \\multicolumn{5}{c||}{".$file1_basename."} & ";
print "\\multicolumn{5}{c|}{".$file2_basename."} \\\\ \n";
print "Example & \n";
print "\\c{Zones} & \\c{States} & \\c{Stack} & \\c{Memory} & \\cc{Time} & \n";
print "\\c{Zones} & \\c{States} & \\c{Stack} & \\c{Memory} & \\c{Time} ";
print "\\\\ \\hline \\hline \n\n";

# Open the files
open(FILE1,@ARGV[0]);
open(FILE2,@ARGV[1]);

# Global variable that is set to 1 when either file1 or file2 are 
# at EOF.  ASSUMES: Both files are the same size, or it truncates to the
# the smallest file.n
$done = 0;

# These are lists of the lines read from each file broken up according
# to the ' ' (space) character.
@file1_words = ();
@file2_words = ();


## Read a line from each file.
&get_file1_line;
&get_file2_line;

do {

    ## If the state count is ever different, then highlight the
    ## the table entry with \textcolor{red}{} command
    if ( @file1_words[2] != @file2_words[2] ) {
	@file1_words[2] = "\\textcolor{red}{".@file1_words[2]."}";
	@file2_words[2] = "\\textcolor{red}{".@file2_words[2]."}";
    }
    ## Print the data from the first line sparated by the Latex '&' for
    ## the table format.
    print join( " & ", @file1_words );
    ## Delete the first entry from the file2_words list, since it is
    ## the file name, and we have already printed it in the above command.
    delete @file2_words[0];
    ## Dump the data in the file2_words
    print join( " & ", @file2_words )." \\\\ \\hline \n";
    ## Read in a new line from each file.
    &get_file1_line;
    &get_file2_line;

}while( !($done) ); #Truncates to the smalled file.
close( FILE1 );
close( FILE2 );

# Print table footer information
print "\\end{longtable}} \n";
print "\\end{center} \n";
#print "\\label{tab:comp} \n";
#print "\\end{table} \n";
print "%%                         END TABLE                      %%\n";
print "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
print "\\end{document} \n";

sub get_file1_line {
    # Get a line from the file
    if ( !($file1_line = <FILE1>) ) { 
	$done = 1; 
    }
    else {
	@file1_words = split(/\s+/,$file1_line);
	# Replace all '_' characaters with '\_'
	@file1_words[0] =~ s#_+#\\_#g;
	if ( $#file1_words == 0 ) {
	    @file1_words = ( @file1_words[0], "DNF", "DNF", "DNF", 
			     "DNF", "DNF" );
        }
    }
}

sub get_file2_line {
    # Get a line from the file
    if ( !($file2_line = <FILE2>) ) {
	$done = 1; 
    }
    else {
	@file2_words = split(/\s+/,$file2_line);
        # Replace all '_' characaters with '\_'
	@file2_words[0] =~ s#_+#\\_#g;
	if ( $#file2_words == 0 ) {
	    @file2_words = ( @file2_words[0], "DNF", "DNF", "DNF", 
			     "DNF", "DNF" );
	}
    }
}
