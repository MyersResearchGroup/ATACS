eval "set PATH=/usr/bin:/usr/local/bin:/Perl/bin:${PATH};exec perl -S $0 $*"
    if $running_under_some_shell; #above line won't run if already in Perl

use English; # Allow the long names for perl's built-in variables.
$pattern = shift;

foreach $infile (@ARGV){
    open (IN,"$infile") || die "Couldn't open $infile: $!";
    $last = $ARG while (<IN>);
    print "$infile:$last" if $last =~ /$pattern/;
    close(IN);
}
