#!/usr/bin/perl
use float; #See float.pm for details

if ($#ARGV < 1){
  print "Usage ./goodbinsort.pl <input file> <# of bins> {more vars}\n";
  exit(0);
}

open (IN, $ARGV[0]) or die "I cannot open the input file $ARGV[0]\n";
#The data file that contains a table of transient data points
my @in = <IN>;
close IN;

$output_name = $ARGV[0];
$output_name =~ s/.dat/.sort/;
$bin_name = $ARGV[0];
$bin_name =~ s/.dat/.bins/;
open OUT, ">$output_name";
open BIN, ">$bin_name";
$iterations = 10;

if ($#ARGV > 1){ #If each variable has an explicitly stated number of bins
  my @temp = split (/\s+/,$in[0]);
  my $vars = $#temp+1; #Find out how many variables there are
  $bin_flag = 1;
  for (my $i = 1; $i <= $vars; $i++){ #For each of those variables
    $bins[$i] = $ARGV[$i] - 1; #Copy the number of bins for each variable
  }
}
else{ #If all the variables have the same number of bins.
  $bins = $ARGV[1] - 1;
}

for my $i(0..$#in){ #For each line of the data table
  $in[$i] =~ s/\s+$//; #Remove the trailing white space
  my @temp = split (/\s+/,$in[$i]); #Create an array of the variable values
  $time[$i] = $temp[0]; #Save the time in an array to be used for rates
  $vars = $#temp-1;
  for (my $j=1; $j<=$#temp; $j++){ #For each variable (other than time)
    $table[$i][$j-1] = $temp[$j]; #Enter the data value into a table
  }
}

for my $i(0..$vars){ #For each variable
  for my $j(0..$#in){ #For each data point in that variable
    if ($min[$i] eq ""){ #Find the minimum value for that variable
      $min[$i] = $table[$j][$i];
    }
    elsif (compare($min[$i],$table[$j][$i]) > 0){
      $min[$i] = $table[$j][$i];
    }
    if ($max[$i] == ""){ #Also find the maximum value for that variable
      $max[$i] = $table[$j][$i];
    }
    elsif (compare($max[$i],$table[$j][$i]) < 0){
      $max[$i] = $table[$j][$i];
    }
  }
  if ($bin_flag == 1){ #If each variable has a different number of bins
    #Divide the data points into equal bin sizes
    my $diff = subtract($max[$i],$min[$i]);
    my $step = int_divide($diff,$bins[$i]);
    my $temp = $min[$i];
    for (my $j = 0; $j <= $bins[$i]; $j++){
      $divide[$j][$i] = $temp;
      $temp_divide[$j][$i] = $temp;
      $temp = add($temp,$step);
    }
  }
  else{
    #Do the same thing with the same number of bins for each variable
    my $diff = subtract($max[$i],$min[$i]);
    my $step = int_divide($diff,$bins);
    my $temp = $min[$i];
    for (my $j = 0; $j <= $bins; $j++){
      $divide[$j][$i] = $temp;
      $temp_divide[$j][$i] = $temp;
      $temp = add($temp,$step);
    }
  }
}

&encode_vals; #See subroutine for details

my $k = 0;
#Avoids erroneous rates when multiple transient runs are concatonated
for (my $i=0; $i<$#table; $i++){ #For each line of data
  my $k_flag = 0;
  for my $j(0..$vars){ #For each variable
    #Calculate the rate from one time point to the next
    my $temp_diff = subtract($table[$i+1][$j],$table[$i][$j]);
    my $temp_time = subtract($time[$i+1],$time[$i]);
    if (compare($temp_time,0.000e+00) >= 0){
      $rate[$k][$j] = divide($temp_diff,$temp_time);
      #Also encodes the data according to whether each variable is
      #increasing or decreasing at a certain time value
      if ($rate[$k][$j] =~ /^-/){
        $temp_array[$j] = 0;
      }
      else{
        $temp_array[$j] = 1;
      }
      #Save the rate encoding in this array (Each line is one time point)
      $rate_encoded[$k] = join "", @temp_array;
      $k_flag = 1;
    }
  }
  if ($k_flag == 1){
    $k++;
  }
}
          
my $count = 0;
foreach $line(@val_encoded){ #For each line of the value encoded data
  if($flag{$line} != 1){
    $vals[$count] = $line; #Creates an array that removes redundant encodings
    #Hence, each line of $vals represents one place in a LHPN
    $flag{$line} = 1;
    $count++;
  }
}

my $temp_int = 1; #Increases step size if the change is not
                  #sufficiently large
for my $i(0..$iterations){ #For a predetermined number of iterations
  for my $j(0..$#divide){ #For each dividing line
    for my $k(0..$vars){
      &encode_vals;
      my $init_score = &score; #See subroutine for details
      for my $j(0..$#divide){ #Make a copy of the table
        for my $k(0..$vars){
          $temp_divide[$j][$k] = $divide[$j][$k];
        }
      }
      #Calculate the distance each line ought to move
      #The line moves equally each iteration so that at the end of the
      #program, it has moved the entire distance to the next line
      my $epsilon = subtract($divide[$j+1][$k],$divide[$j][$k]);
      $epsilon = int_divide($epsilon,$iterations);
      $epsilon = int_multiply($epsilon,$temp_int);
      #Move the line
      $temp_divide[$j+1][$k] = subtract($divide[$j+1][$k],$epsilon);
      encode_vals(temp); #Sort the data into bins again
      my $all_score = &score; #Calculate whether the new division helps
      if ($all_score > $init_score){ #If it does
        #plot_vals($i);
        for my $j(0..$#temp_divide){ #Make it permanent
          for my $k(0..$vars){
            $divide[$j][$k] = $temp_divide[$j][$k];
          }
        }
        $temp_int = 1;
      }
      else{
        #plot_vals($i);
        $temp_int++; #Increase the distance that the line moves
      }
    }
  }
  &encode_vals; #Sort the values according to whichever bins are permanent
}

for my $i(0..$#in){ #Print the data table and encoding to output
  print OUT "$in[$i] ";
  my @temp = split //, $val_encoded[$i];
  print OUT "@temp\n";
}

for my $row(@divide){ #Print the divisions between the lines to output
  print BIN "@$row\n";
}

close OUT;
close BIN;

sub score{
  #Finds the percentage of points in each bin that has the same
  #boolean rate encoding as the majority encoding in that same bin
  my $a = $_[0]; #The encoding of a specific bin in which to calculate
                 #the score (no longer used)
  my $b = $a;
  my $count = 0; #The number of data points in a bin
  my $eq_count = 0; #The number of points of one rate encoding
  for my $i(0..$#vals){ #For each bin
    foreach my $key(keys %count){ #Reset the count of rate_encoded values
      $count{$key} = 0;
    }
    if ($a eq ""){ #Sets which bin in which the score will be calculated
      $b = $vals[$i];
    }
    elsif ($i > 1){ #If only one bin is to be scored, then it skips
                    #the rest of the iterations
      last;
    }
    for my $j(0..$#rate_encoded){ #For each data point
      if ($b == $val_encoded[$j]){ #If it is in the right bin
        my $line = $rate_encoded[$j];
        if ($count{$line} == "" || $count{$line} == 0){
          $count{$line} = 1; #Counts the number of data points in that
                             #bin with a certain rate encoding
        }
        else{
          $count{$line}++;
        }
      }
    }
    my @max = ("default",0);
    foreach my $key(keys %count){ #Finds the rate encoding with the
                                #maximum number of data points in a
                                #give bin
      if ($count{$key} > $max[1]){
        @max = ($key,$count{$key});
      }
    }
    for my $j(0..$#rate_encoded){ #Finds the number of data points with
                                #the most prominent rate encoding
      my $line = $rate_encoded[$j];
      if ($b == $val_encoded[$j]){
        $count++; #Increments the number of data points in that bin
        if ($line == $max[0]){
          $eq_count++; #Increments the number of data points with the
                       #most prominent encoding
        }
      }
    }
  }
  if ($count == 0){
    print "Warning: $a is an empty bin\n";
    return;
  }
  my $score = $eq_count/$count; #Calculates the percentage of data
                                #points that agree with the most
                                #prominent encoding
  $score *= 100;
  return $score;
}

sub encode_vals{
  #Encodes each data point into a series of digits, each representing
  #a bin encoding dependent on the value of the variable (Each line is
  #one time point)
  my $flag = 0;
  my @array = [0,0];
  if ($_[0] eq 'temp'){ #Sets flag to 1 if it should use the temp
                        #divisions instead of the permanent ones
    $flag = 1;
  }
  for my $i(0..$#table){ #For each data point
    for my $j(0..$vars){ #For each variable value
      if ($bin_flag == 1){ #If there are multiple numbers of bins
        $bins = $bins[$j]; #Set the "bins" value to the pertinent one
      }
      my $a = $table[$i][$j]; #Set a temp variable for conciseness
      for my $k(0..$#divide){ #For each 1D bin
        if ($flag == 1){
          $c = $temp_divide[$k][$j]; #Use the temp divisions
        }
        else{
          $c = $divide[$k][$j]; #Or the permanent ones
        }
        if (compare($a,$c) <= 0){ #If the data point falls below the
                                #division line
          $array[$j] = $k-1; #Set the encoding according to that
                             #division line
          last;
        }
      }
      if ($array[$j] eq ""){ #If there still isn't an encoding
        if ($bins > 0){
          $array[$j] = $bins; #Assign the maximum as a default
        }
        else{
          $array[$j] = 0;
        }
      }
      elsif ($array[$j] <= -1){ #If an encoding is less than zero
        $array[$j] = 0; #Set it to zero (this is not an error)
      }
    }
    $val_encoded[$i] = join "", @array; #Codify the values into one
                                #array line
  }
}

sub find_min{
  #Currently not used.  There's no good reason for it still to be here.
  @min_score = ("default", 101);
  foreach $line(@vals){
    if (score($line) ne "" && score($line) < $min_score[1]){
      @min_score = ($line, score($line));
    }
  }
}

sub plot_vals{
  #Exists only for testing purposes, creates a plot of each different
  #arrangement of bins and points
  my $i = $_[0];
  for my $j(0..4){
    my $inda = 0;
    if ($j > 2){
      $inda = 1;
    }
    my $indb = $j%2;
    if ($indb == 0){
      $indb = 2;
    }
    open LINES, "lines$j";
    my @lines = <LINES>;
    close LINES;
    open LINES, ">lines$j";
    foreach my $line(@lines){
      my @temp_array = split /\s+/, $line;
      my $temp_val = etof($temp_divide[$indb][$inda]);
      $temp_array[$inda] = $temp_val;
      print LINES "@temp_array\n";
    }
    close LINES;
  }
  system "cp plot.gnuplot TMPFILE";
  open (PLOT, "TMPFILE") or die "I can't open file\n";
  my @plot = <PLOT>;
  close PLOT;
  foreach my $line (@plot){
    $line =~ s/\s+$//;
    my $temp = "\"plot$i.ps\"";
    $line =~ s/filename/$temp/;
  }
  open PLOT, ">TMPFILE";
  foreach my $line(@plot){
    print PLOT "$line\n";
  }
  close PLOT;
  system "chmod a+x TMPFILE";
  system "./TMPFILE";
}
