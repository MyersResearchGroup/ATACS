#!/usr/bin/perl

if ($#ARGV != 1){
  print "Usage ./convert.pl <input file> <output file>\n";
  exit(0);
}

open (IN, $ARGV[0]) or die "I cannot open input file $ARGV[0]\n";
my @in = <IN>;
close IN;

open OUT, ">$ARGV[1]";

foreach my $line(@in){
  $line =~ s/\s+$//; #Remove the trailing whitespace
  my @array = split (/\s+/,$line);
  push @copy, [@array]; #Make a table of the input
}

$vars = $copy[4][2]; #Copy the number of variables
$points = $copy[5][2]; #And the number of data points
#From the proper place in the table of a SPICE output file

for (my $i = 0; $i < 8+$vars; $i++){
  shift @copy; #Remove the header lines
}

for (my $i = 0; $i < $#copy; $i++){
  $indexa = int $i/($vars+1); #Calculate which line is with which variable
  $indexb = $i % ($vars+1); #Calculate in which column each value goes
  if ($indexb == 0){
    $table[$indexa][$indexb] = $copy[$i][2]; #Place the proper value
                                             #in the proper position
  }
  else{
    $table[$indexa][$indexb] = $copy[$i][1]; #Do the same thing on
                                      #the lines in which the index
                                      #number is the first value
  }
}

for $row(@table){
  print OUT "@$row\n"; #Print the table to output
}

close OUT;
