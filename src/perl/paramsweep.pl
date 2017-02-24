#!/usr/bin/perl

if ($#ARGV != 0){
  print "Usage ./paramsweep.pl <input netlist>\n";
  exit(0);
}

open (IN, $ARGV[0]) or die "I cannot open input file $ARGV[0]\n";
my @in = <IN>;
$input_name = $ARGV[0];
$input_name =~ s/\.cir$//; #Used to name the output files

$nest_levels = -1;
foreach my $line(@in){ #Scans the input file for pertinent syntax
  if ($line =~ /iteration/){ #User given number of transient simulations
    my @array = split (/\s+/,$line);
    $iterations = $array[0];
    $iterations =~ s/\*//;
  }
  elsif ($line =~ /^\.define/){ #Lines that define the parameters
    my @array = split (/\s+/,$line);
    my $temp = "";
    if ($array[3] =~ /^\*/){ #If the line has pertinent syntax
      $temp = $array[3]; #Save the instructions for the parameter sweep
      $temp =~ s/^\*//;
    }
    else{ #If there is a define not being used for a parameter sweep
      print "Warning: No assignment instructions for $line\n";
      next;
    }
    if ($temp =~ /^sweep/){ #If the parameter sweeps through a given range
      #Note: If more iterations are run than there are elements in the
      #sweep, then the sweep cycles through its elements
      push @sweep_names, $array[1];
      $temp =~ s/^sweep\(//;
      $temp =~ s/\)$//;
      my @temp_args = split (/\,/,$temp); #Isolates the arguments in
                                #the instructions
      my $temp_iter = ($temp_args[2]-$temp_args[1])/$temp_args[3];
      #Calculates the number of iterations to be used
      my $temp_int = int $temp_iter;
      if ($temp_iter - $temp_int != 0){
        $temp_iter = $temp_int + 1; #Rounds the number of iterations
      }
      else{
        $temp_iter = $temp_int;
      }
      if ($temp_args[0] > $nest_levels){ #Finds the number of nesting levels
        $nest_levels = $temp_args;
      }
      if ($temp_iter > $nest_iter[$temp_args[0]]){ #Finds the number
                                #of iterations for each nesting level
        $nest_iter[$temp_args[0]] = $temp_iter;
      }
      push @sweep_args, [@temp_args]; #Creates a table of the
                                #arguments for each parameter
    }
    elsif ($temp =~ /^uniform/){
      push @uni_names, $array[1];
      $temp =~ s/^uniform\(//;
      $temp =~ s/\)$//;
      #Splits the arguments for parameters in which a uniformly
      #distributed random value is to be generated
      my @temp_args = split (/','/,$temp);
      push @uni_args, [@temp_args]; #Saves those arguments in a table
    }
    elsif (@temp =~ /^normal/){
      push @gauss_names, $array[1];
      $temp =~ s/^normal\(//;
      $temp =~ s/\)$//;
      #Splits the arguments for parameters in which a normally
      #distributed random value is to be generated
      my $temp_args = split (/','/,$temp);
      push @gauss_args, [@temp_args]; #Saves those arguments in an array
    }
  }
}

foreach $line(@nest_iter){
  $total_iter += $line;
  #Calculates the total number of iterations needed to use all sweep values
}
if ($iterations < $total_iter){
  #Displays a warning if all sweep values will not be used
  print "Warning: Sweeps will not be completed; ";
  print "$total_iter iterations needed\n";
}

for my $i(0..$iterations){ #For each transient simulation
  open TMP, "> TMPFILE";
  print TMP "@in";
  for my $j(0..$#uni_names){ #Calculate a random parameter value
    my $min = $uni_args[$j][0];
    my $max = $uni_args[$j][1];
    my $diff = $max - $min;
    my $val = rand($diff) + $min;
    my @temp_array = ($uni_names[$j], $val);
    push @uni_array, [@temp_array]; #Save that value in an array
  }
  for my $j(0..$#gauss_names){ #Calculate a gaussian parameter value
    my $mean = $gauss_args[$j][0];
    my $stdev = $gauss_args[$j][1];
    my $val = gaussian_rand*$stdev+$mean;
    my @temp_array = ($gauss_names[$j], $val);
    push @gauss_array, [@temp_array]; #Save that value in an array
  }
  for my $j(0..$#sweep_names){ #Calculate the next sweep value to be used
    my $level = $sweep_args[$j][0];
    my $min = $sweep_args[$j][1];
    my $max = $sweep_args[$j][2];
    my $step = $sweep_args[$j][3];
    my $temp_iter = ($max-$min)/$step;
    my $temp_int = int $temp_iter;
    if ($temp_iter - $temp_int != 0){
      #Determine which sweep value to use based on the nesting level
      $temp_iter = $temp_int + 1;
    }
    else{
      $temp_iter = $temp_int;
    }
    my $temp_i = $i;
    for my $k(0..$level){
      $temp_i -= $nest_iter[$k];
    }
    $temp_i = $temp_i % $temp_iter;
    my $val = $min + $temp_i*$step;
    if ($val > $max){
      $val = $max;
    }
    my @temp_array = ($sweep_names[$j], $val);
    push @sweep_array, [@temp_array]; #Save the value in an array
  }
  for my $j(0..$#sweep_array){ #Copy the sweep values into the netlist
    $name = $sweep_array[$j][0];
    $value = $sweep_array[$j][1];
    system "sed 's/$name/$value/g' TMPFILE >NewTemp";
    system "cp NewTemp TMPFILE";
  }
  for my $j(0..$#uni_array){ #Copy the uniform random values into the netlist
    $name = $uni_array[$j][0];
    $value = $uni_array[$j][1];
    system "sed 's/$name/$value/g' TMPFILE >NewTemp";
    system "cp NewTemp TMPFILE";
  }
  for my $j(0..$#gauss_array){ #Copy the normal random values into the netlist
    $name = $gauss_array[$j][0];
    $value = $gauss_array[$j][0];
    system "sed 's/$name/$value/g' TMPFILE >NewTemp";
    system "cp NewTemp TMPFILE";
  }
  system "rm NewTemp";
  system "./ngspice -r $input_name$i.out -b TMPFILE"; #Run the SPICE simulation
  system "./ngsconvert b $input_name$i.out a $input_name$i.ascii";
  #Convert the binary output into ascii format
  close TMP;
}
close IN;

sub gaussian_rand {
  #To be honest, I don't know how this works.  I copied it off of the internet
    my ($u1, $u2);  # uniformly distributed random numbers
    my $w;          # variance, then a weight
    my ($g1, $g2);  # gaussian-distributed numbers

    do {
        $u1 = 2 * rand() - 1;
        $u2 = 2 * rand() - 1;
        $w = $u1*$u1 + $u2*$u2;
    } while ( $w >= 1 );

    $w = sqrt( (-2 * log($w))  / $w );
    $g2 = $u1 * $w;
    $g1 = $u2 * $w;

    return $g1;
}
