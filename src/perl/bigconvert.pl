#!/usr/bin/perl

if ($#ARGV < 1){
  print "Usage ./bigconvert.pl <output file> <input files>\n";
  exit(0);
}

for (my $i = 1; $i <= $#ARGV; $i++){
  open (IN, $ARGV[$i]) or die "I cannot open input file $ARGV[0]\n";
  my @temp = <IN>; #Create a table of input files
  #Each row is a file, each column is a line of that file
  push @in, [@temp];
  close IN;
}

open OUT, ">$ARGV[0]"; #Open the output file

for my $i(0..$#in){
  @copy = "";
  foreach my $line(@{$in[$i]}){ #Make a copy of each file individually
    $line =~ s/\s+$//; #Remove trailing whitespace
    my @array = split (/\s+/,$line);
    push @copy, [@array];
  }
  $vars = $copy[5][2]; #Copy the number of variables
  $points = $copy[6][2]; #And the number of data points
  #From their appropriate positions in the SPICE output file
  for (my $j = 0; $j <= 8+$vars; $j++){
    shift @copy; #Shift the header lines off of the data points
  }
  for (my $j = 0; $j <= $#copy; $j++){
    my $indexa = int $j/($vars+1); #Calculate which line is with which variable
    my $indexb = $j % ($vars+1); #Calculate in which column each value goes
    if ($indexb == 0){
      $table[$indexa][$indexb] = $copy[$j][2]; #Place the proper value
                                               #in the proper position
    }
    else{
      $table[$indexa][$indexb] = $copy[$j][1]; #Do the same thing on
                                      #the lines in which the index
                                      #number is the first value
    }
  }
  for my $row(@table){
    print OUT "@$row\n"; #Print the table to output
  }
}

close OUT;
