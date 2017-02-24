#!/usr/bin/perl
use float;

if ($#ARGV > 1){
  print "Usage ./graphIt.pl (-ersdb) <input file>\n";
  exit;
}
elsif ($ARGV[0] =~ /^-h/){
  print "**********\n";
  print "Usage ./graphIt.pl (-ersdb) <input file>\n";
  print "-e Prints enabling conditions\n";
  print "-r Prints average rates for variables\n";
  print "-s Prints min and max rates for variables\n";
  print "-d Prints delay values for transitions\n";
  print "-b Prints boolean assignments for encoded place names\n";
  print "**********\n";
  exit;
}
elsif ($ARGV[0] =~ /^-/){
  open (IN, $ARGV[1]) or die "I cannot open the input file $ARGV[1]\n";
  my $temp = $ARGV[1];
  $temp =~ s/.sort/.bins/;
  open (BINS, $temp) or die "I cannot open the file $temp\n";
  $temp =~ s/.bins/.g/;
  open OUT, ">$temp";
  if ($ARGV[0] =~ /e/){
    $enab_flag = 1;
  }
  if ($ARGV[0] =~ /r/){
    $rate_flag = 1;
  }
  if ($ARGV[0] =~ /s/){
    $span_flag = 1;
  }
  if ($ARGV[0] =~ /d/){
    $delay_flag = 1;
  }
  if ($ARGV[0] =~ /b/){
    $bool_flag = 1;
  }
}
else{
  open (IN, $ARGV[0]) or die "I cannot open the input file $ARGV[0]\n";
  my $temp = $ARGV[0];
  $temp =~ s/.sort/.bins/;
  open (BINS, $temp) or die "I cannot open the file $temp\n";
  $temp =~ s/.bins/.g/;
  open OUT, ">$temp";
}
my @in = <IN>;
my @bins_in = <BINS>;
close IN;
close BINS;

foreach my $i(0..$#bins_in){
  #Creates a table of the values used for enabling conditions
  $bins_in[$i] =~ s/\s+$//;
  my @temp = split(/\s+/,$bins_in[$i]);
  for my $j(0..$#temp){
    $bin_table[$i][$j] = $temp[$j];
  }
}

foreach my $line (@in){
  #Creates tables of the input file, with and without repeated places
  $line =~ s/\s+$//;
  my @temp = split (/\s+/,$line);
  $vars = $#temp/2; #Calculates and saves the number of variables
  my $flag = 0;
  for (my $j = $vars+1; $j <= $#temp; $j++){
    if ($temp[$j] != $old_temp[$j]){ #Skips repeated places
      last;
    }
    if ($j == $#temp){
      $flag = 1;
    }
  }
  if ($flag == 0){
    push @table, [@temp]; #Without repeated places
  }
  push @rate_table, [@temp]; #With repeated places (used to calculate rates)
  @old_temp = @temp;
}

for (my $i = 0; $i < $#table; $i++){
  #Creates a table with just the encoded values, and an array of the
  #time values
  for (my $j = $vars; $j < 2*$vars; $j++){
    $bins[$i][$j-$vars] = $table[$i][$j+1]; #Encoded values
    $time[$i] = $table[$i][0]; #Array of time vals
  }
}

for (my $i = 0; $i < $#rate_table; $i++){
  #Creates a table like above, but with repeated places
  for (my $j = $vars; $j < 2*$vars; $j++){
    $rate_bins[$i][$j-$vars] = $rate_table[$i][$j+1];
    $rate_locs[$i] = join "", @{$rate_bins[$i]};
  }
}

if ($bool_flag == 1){
  #Finds the number of bits needed to encode each variable
  bool_list();
}

for (my $i = 0; $i <= $#bins; $i++){
  #Boolean encoding of bin values
  for (my $j = 0; $j < $vars; $j++){
    $temp_bins[$i][$j] = $bins[$i][$j];
    for (my $k = $max[$j]; $k >= 0; $k--){ #Looping for each bit
      if ($temp_bins[$i][$j] >= 2**$k){ #If next bit should be a 1
        $bit_bins[$i][$j] .= 1;
        $temp_bins[$i][$j] -= 2**$k;
      }
      else{
        $bit_bins[$i][$j] .= 0; #Stores in 2D array
      }
    }
  }
}

for my $i(0..$#bins){
  #Creates places based on the bin values and places them in an array
  #Changes 2D arrays to 1D arrays
  $bin_locs[$i] = join " ", @{$bins[$i]};
  $locs[$i] = join "", @{$bins[$i]};
  $bit_locs[$i] = join "", @{$bit_bins[$i]};
}

$j = 0;
$k = 0;
for (my $i = 1; $i <= $#in; $i++){ #For each line of the input file
  my $index = sprintf "%s%s", $rate_locs[$i-1], $rate_locs[$i];
  for (my $l = 1; $l <= $vars; $l++){
    #Creates tables for the maximum, minimum, and average rates for
    #each transition
    my $temp_diff = subtract($rate_table[$i][$l],$rate_table[$i-1][$l]);
    my $time_diff = subtract($rate_table[$i][0],$rate_table[$i-1][0]);
    if (compare($time_diff,0.0000e+00) <= 0){
      last;
    }
    my $temp_rate = divide($temp_diff,$time_diff);
    my $rate_ind = sprintf "%s%s", $index, $l-1;
    #Note: rate_ind is a concatonation of the transition name and the
    #variable name to which the rates apply
    my $a = $min_rate{$rate_ind};
    my $b = $max_rate{$rate_ind};
    $rates[$i-1][$l-1] = $temp_rate;
    if (($a eq "") || (compare($temp_rate,$a) > 0)){
      $max_rate{$rate_ind} = $temp_rate; #Saves the maximum rate for
                                #each transition and variable
    }
    if (($b eq "") || (compare($temp_rate,$b) < 0)){
      $min_rate{$rate_ind} = $temp_rate; #Saves the minimum rate for
                                #each transition and variable
    }
    for (my $m = 0; $m <= $k; $m++){ #Calculates the average rate for
                                #each transition and variable
      if ($avg_sum{$rate_ind} == ""){ 
        $avg_sum{$rate_ind} = $temp_rate;
      }
      else{ #Adds the rates for the rate index
        $avg_sum{$rate_ind} = add($avg_sum{$rate_ind},$temp_rate);
      }
      if ($avg_tally{$rate_ind} == ""){
        $avg_tally{$rate_ind} = 1; #Tallys the number of rates added
                                #under the rate index
      }
      else{
        $avg_tally{$rate_ind}++;
      }
      $avg_rate{$rate_ind} = int_divide($avg_sum{$rate_ind},$avg_tally{$rate_ind});
      #Calculates the average by dividing the sum by the tally
    }
  }
  $k++;
  if ($flag{$index} != 1){
    #Creates a hash of all possible transition names, with relevant delays
    #$trans{$index} = sprintf "t%s", $index;
    $flag{$index} = 1;
    if ($time[$i] != 0){
      $delay[$j] = subtract($time[$i],$time[$i-1]); #Calculates the
                                #delays (only used if rates and
                                #enabling conditions are not)
    }
    $j++;
    $k = 0;
  }
  else{
    $delay[$j-1] = sprintf "%s %s", $delay[$j-1],subtract($time[$i],$time[$i-1]);
  }
}
if ($delay_flag == 1){
  #Finds the maximum and minimum delays for each transition
  for my $i(0..$#delay){
    my @temp_del = split (/\s+/,$delay[$i]);
    my $max_del = $temp_del[0];
    my $min_del = $temp_del[0];
    foreach $row(@temp_del){
      if (compare($row,$max_del) < 0){
        $max_del = $row;
      }
      if (compare($row,$min_del) > 0){
        $min_del = $row;
      }
    }
    $delay[$i] = sprintf "%s %s", $min_del, $max_del;
    #Concatonates the maximum and minimum delays into one string
  }
}

my $j = 0;
for (my $i = 0; $i < $#locs; $i++){
  my $temp = join " ", $locs[$i], $locs[$i+1];
  if ($flag{$temp} != 1){ #Creates a concatonation of the bin
                          #encodings for use in place names
    $new_loc[$j] = join " ", $locs[$i], $locs[$i+1];
    $new_bits[$j] = join " ",$bit_locs[$i],$bit_locs[$i+1];
    my @temp = split (/\s+/,$new_loc[$j]);
    my $index = join "", @temp;
    if ($place{$locs[$i]} eq ""){
      $place{$locs[$i]} = sprintf "p%s", $i;
    }
    if ($place{$locs[$i+1]} eq ""){
      $place{$locs[$i+1]} = sprintf "p%s", $i+1;
    }
    if ($trans{$index} eq ""){ #Concatonates place names for
                               #transition names
      $trans{$index} = sprintf "t%sto%s", $place{$locs[$i]}, $place{$locs[$i+1]};
    }
    $flag{$new_loc[$j]} = 1;
    $j++;
  }
}

if ($enab_flag == 1){
  #Creates a table of enabling conditions
  for my $i(0..$#locs){
    my @old = @temp;
    @temp = split(/\s+/,$bin_locs[$i]);
    for my $j(0..$#temp){
      if ($old[$j] == $temp[$j] || $old[$j] eq ""){
        $enab_bins[$i][$j] = "\0";
      }
      elsif ($old[$j] > $temp[$j]){ #If the variable goes above a threshold
        $enab_bins[$i][$j] = "<=";
        if ($bin_table[$temp[$j]-1][$j+1] =~ /e/){ #Copy the ennabling
                                #value from the table made earlier
          $enab_bins[$i][$j] .= etof($bin_table[$temp[$j]][$j]);
        }
        else{
          $enab_bins[$i][$j] .= $bin_table[$temp[$j]-1][$j];
        }
      }
      elsif ($old[$j] < $temp[$j]){ #If the variable goes below a threshold
        $enab_bins[$i][$j] =  ">=";
        if ($bin_table[$temp[$j]-1][$j+1] =~ /e/){
          $enab_bins[$i][$j] .= etof($bin_table[$temp[$j]-1][$j+1]);
        }
        else{
          $enab_bins[$i][$j] .= $bin_table[$temp[$j]-1][$j+1];
        }
      }
    }
  }
}

#Normalizes the units to reasonable values
var_units();
rate_units();

#Changes float values to integers
for my $key (keys %min_rate){
  $min_rate{$key} = ftoi($min_rate{$key});
}
for my $key (keys %max_rate){
  $max_rate{$key} = ftoi($max_rate{$key});
}
for my $key (keys %avg_rate){
  $avg_rate{$key} = ftoi($avg_rate{$key});
}

if ($enab_flag == 1){
  #Prints enabling conditions in the form needed for the output
  #Concatonates the variable names and different enabling conditions
  #for each transition
  for (my $i = 0; $i < $#enab_bins; $i++){
    my $index = sprintf "%s%s", $locs[$i], $locs[$i+1];
    #$index is a concatonation of the place names
    my $flag = 0;
    for (my $j = 0; $j < $vars; $j++){
      if ($enab_bins[$i][$j] =~ /e/){
        $enab_bins[$i][$j] = ftoi($enab_bins[$i][$j]);
      }
    }
    for (my $j = 0; $j < $vars; $j++){
      if ($enab_bins[$i+1][$j] ne "\0" && $flag == 0){
        $enab_line{$index} = sprintf "v%d%s", $j, $enab_bins[$i+1][$j];
        $flag = 1;
      }
      elsif ($enab_bins[$i+1][$j] ne "\0" && $flag == 1){
        $enab_line{$index} .= sprintf "&v%d%s", $j, $enab_bins[$i+1][$j];
      }
    }
  }
}

#Here to end prints to output file

if ($bool_flag == 1){
  #Prints the boolean variables to output
  print_bool();
}

print OUT ".dummy ";
foreach my $key (keys %trans){
  #Prints dummy list of transitions
  print OUT "$trans{$key} ";
}

if ($rate_flag == 1 || $span_flag == 1 || $enab_flag == 1){
  #Prings continuous variables
  print OUT "\n#@.variables ";
  for (my $i = 0; $i < $vars; $i++){
    print OUT "v$i ";
  }
}

if ($bool_flag == 1){
  #Prints boolean encoding of each initial state
  for my $i(0..$#bit_locs){
    #$flag{$bit_locs[$i]} = 0;
    if(compare($table[$i][0],0.000e+00) == 0 && $flag{$bit_locs[$i]} != 1){
      print OUT "\n#@.init_state [$bit_locs[$i]]";
      $flag{$bit_locs[$i]} = 1;
    }
  }
}
print OUT "\n";
foreach my $key(keys %place){
  print OUT "#$place{$key} = $key\n";
}
print OUT ".graph\n";
foreach my $row(@new_loc){
  #Prints all places and transitions
  my @temp = split (/\s+/,$row);
  my $index = join "", @temp;
  if ($temp[1] ne ""){
    print OUT "$place{$temp[0]} $trans{$index} #@ [0,0]\n";
    print OUT "$trans{$index} $place{$temp[1]} #@ [0,0]\n";
  }
}

for my $i(0..$#table){
  #Prints initial conditions (not boolean encoded)
  #$pl_flag{$locs[$i]} = 0;
  if ((compare($table[$i][0],0.000e+00) == 0 || $i == 0) && $pl_flag{$locs[$i]} != 1){
    print OUT ".marking {$place{$locs[$i]}}\n";
    $pl_flag{$locs[$i]} = 1;
    if ($rate_flag == 1 || $span_flag == 1){
      for (my $j = 0; $j < $vars; $j++){
        if ($pl_flag{$table[$i][$j+1]} == 1){
          $flag = 1;
          last;
        }
        else{
          $flag = 0;
        }
      }
      if ($flag != 1){
        #Prints initial values of continuous variables
        print OUT "#@.init_vals {";
        for (my $j = 0; $j < $vars; $j++){
          my $temp_table = ftoi($table[$i][$j+1]);
          print OUT "<v$j=$temp_table>";
          $pl_flag{$table[$i][$j+1]} = 1;
        }
        print OUT "}\n";
      }
      #Prints initial rates of continuous variables
      for (my $j = 0; $j < $vars; $j++){
        if ($pl_flag{$rates[$i][$j]} == 1){
          my $flag = 1;
        }
      }
      if ($flag != 1){
        print OUT "#@.init_rates {";
        for (my $j = 0; $j < $vars; $j++){
          my $temp_rate = ftoi($rates[$i][$j]);
          $temp_rate =~ s/inf/9999/; #Changes problematic values
          $temp_rate =~ s/0/1/;
          print OUT "<v$j=$temp_rate>";
          $pl_flag{$rates[$i][$j]} = 1;
        }
        print OUT "}\n";
      }
    }
  }
}

if ($enab_flag == 1){
  #Prints enabling conditions for each transition
  print_enab();
}
if ($rate_flag == 1){
  #Prints rate assignments for each transition, based on the average
  #rates
  for my $i(0..$#locs){
    my @old = @temp;
    @temp = split(/\s+/,$bin_locs[$i]);
    for my $j(0..$#temp){
      if ($old[$j] <= $temp[$j]){
        for my $key(keys %avg_rate){ #Changes zeros to +-1
          if ($avg_rate{$key} =~ m/$temp[$j]$/ && $avg_rate{$key} == 0){
            $avg_rate{$key} = 1;
          }
        }
      }
      else{
        for my $key(keys %avg_rate){
          if ($avg_rate{$key} =~ m/$temp[$j]$/ && $avg_rate{$key} == 0){
            $avg_rate{$key} = -1;
          }
        }
      }
    }
  }
  rate_assign();
}
if ($span_flag == 1){
  #Prints rate assignments for each transition, using the min and max
  #rates
  for my $i(0..$#locs){
    my @old = @temp;
    @temp = split(/\s+/,$bin_locs[$i]);
    my $tmp_ind = join "", @temp;
    for my $j(0..$#temp){
      if ($old[$j] <= $temp[$j]){
        for my $key(keys %max_rate){ #Changes zeros to +-1
          if ($key =~ m/$tmp_ind$j$/ && $max_rate{$key} <= 0){
            if ($min_rate{$key} == 0){
              $min_rate{$key} = 1;
            }
            $max_rate{$key} = 1;
          }
        }
      }
      elsif($old[$j] >= $temp[$j]){
        for my $key(keys %min_rate){
          if ($key =~ m/$tmp_ind$j$/ && $min_rate{$key} >= 0){
            $min_rate{$key} = -1;
            if ($max_rate{$key} == 0){
              $max_rate{$key} = -1;
            }
          }
        }
      }
    }
  }
  span_assign();
}
if ($delay_flag == 1){
  #Prints the min and max delay values for each transition (0's if
  #enabling conditions are used)
  delay_assign();
}
if ($bool_flag == 1){
  #Prints the boolean changes for each transition, in which every
  #place is encoded as several boolean variables
  bool_assign();
}

if ($rate_flag == 1 || $span_flag == 1 || $enab_flag == 1){
  #Prints the continuous variables
  print OUT "#@.continuous ";
  for (my $i = 0; $i < $vars; $i++){
    print OUT "v$i ";
  }
  print OUT "\n";
}

print OUT ".end\n";

close OUT;


sub bool_list{
  #Finds the number of bits needed to encode each variable
  @max = (0, 0);
  for (my $j = 0; $j < $vars; $j++){
    for (my $i = 0; $i < $#bins; $i++){
      if ($bins[$i][$j] > $max[$j]){
        $max[$j] = $bins[$i][$j];
      }
    }
    for (my $i = 1; $i <= 16; $i++){
      if (($max[$j] >= 2**($i-1)) && ($max[$j] < 2**$i)){
        $max[$j] = $i-1;
      }
    }
  }
}

sub print_enab{
  #Prints enabling conditions for each transition
  print OUT "#@.enablings {";
  for my $i(0..$#locs){
    my $index = sprintf "%s%s", $locs[$i], $locs[$i+1];
    if ($enab_flag{$index} != 1 && $enab_line{$index} ne ""){
      $enab_flag{$index} = 1;
      print OUT "<$trans{$index}=[$enab_line{$index}]>";
    }
  }
  print OUT "}\n";
}

sub rate_assign{
  #Prints rate assignments for each transition, based on the average rates
  print OUT "#@.rate_assignments {";
  for (my $i = 0; $i < $#new_loc; $i++){
    my @temp = split (/\s+/,$new_loc[$i]);
    my $index = join "", @temp;
    if ($trans{$index} eq ""){
      last;
    }
    my $rate_flag = 0;
    for my $key(keys %avg_rate){
      if ($key =~ /$index/){
        if ($avg_rate{$key} ne ""){
          $rate_flag = 1;
          last;
        }
      }
    }
    if ($rate_flag == 0){
      last;
    } 
    print OUT "<$trans{$index}=[";
    my $flag = 0;
    for (my $j = 0; $j < $vars; $j++){
      my $rate_ind = sprintf "%s%s", $index, $j;
      if ($flag == 1){
        print OUT ";";
      }
      $flag = 1;
      if ($avg_rate{$rate_ind} ne ""){
        print OUT "v$j:=$avg_rate{$rate_ind}";
      }
    }
    print OUT "]>";
  }
  print OUT "}\n";
}

sub span_assign{
  #Prints rate assignments for each transition, using the min and max rates
  print OUT "#@.rate_assignments {";
  my %flag;
  for (my $i = 0; $i < $#new_loc; $i++){
    my @temp = split (/\s+/,$new_loc[$i]);
    my $index = join "", @temp;
  #for my $index (keys %trans){
    if ($trans{$index} eq "" || $flag{$index} == 1){
      next;
    }
    my $rate_flag = 0;
    for my $key(keys %min_rate){
      if ($key =~ /$index/){
        if ($max_rate{$key} ne ""){
          $rate_flag = 1;
          last;
        }
      }
    }
    if ($rate_flag == 0){
      last;
    }
    my $rate_flag = 0;
    for my $key(keys %max_rate){
      if ($key =~ /$index/){
        if ($max_rate{$key} ne ""){
          $rate_flag = 1;
          last;
        }
      }
    }
    if ($rate_flag == 0){
      last;
    }
    print OUT "<$trans{$index}=[";
    $flag{$index} = 1;
    my $flag = 0;
    for (my $j = 0; $j < $vars; $j++){
      my $rate_ind = sprintf "%s%s", $index, $j;
      if ($flag == 1){
        print OUT ";";
      }
      $flag = 1;
      if ($min_rate{$rate_ind} ne "" && $max_rate{$rate_ind} ne ""){
        print OUT "v$j:=[$min_rate{$rate_ind},$max_rate{$rate_ind}]";
      }
    }
    print OUT "]>";
  }
  print OUT "}\n";
}

sub delay_assign{
  #Prints the min and max delay values for each transition (0's if
  #enabling conditions are used)
  print OUT "#@.delay_assignments {";
  for my $i(0..$#new_loc){
    my @temp = split (/\s+/,$new_loc[$i]);
    my $index = join "", @temp;
    if ($enab_flag == 1){
      print OUT "<$trans{$index}=[0,0]>";
    }
    else{
      my @temp_del = split (/\s+/,$delay[$i]);
      $temp_del[0] = ftoi($temp_del[0]);
      $temp_del[1] = ftoi($temp_del[1]);
      print OUT "<$trans{$index}=[$temp_del[0],$temp_del[1]]>";
    }
  }
  print OUT "}\n";
}

sub print_bool{
  #Prints the boolean variables to output
  print OUT ".outputs ";
  for (my $j = 0; $j < $vars; $j++){
    if ($max[$j] eq "" && $j ne ""){
      print OUT "v", $j, "0 ";
    }
    else{
      for (my $i = $max[$j]; $i >= 0; $i--){
        print OUT "v$j$i ";
      }
    }
  }
  print OUT "\n";
}

sub bool_assign{
  #Prints the boolean changes for each transition, in which every
  #place is encoded as several boolean variables
  print OUT "#@.boolean_assignments {";
  for my $i(0..$#new_loc){
    my @temp = split (/\s+/,$new_loc[$i]);
    my $index = join "", @temp;
    if ($temp[1] eq ""){
      last;
    }
    print OUT "<$trans{$index}=[";
    my @temp_bits = split(/\s+/,$new_bits[$i]);
    my $xor_bits = $temp_bits[0]^$temp_bits[1];
    my @xor_array = split //, $xor_bits;
    my @bit_array = split //, $temp_bits[1];
    my $flag = 0;
    for my $j(0..$#xor_array){
      if ($xor_array[$j] =~ /[^\0]/){
        if ($flag == 1){
          print OUT ";";
        }
        $flag = 1;
        $sum = 0;
        for (my $k = 0; $k <= $#max; $k++){
          if ($j-$sum <= $max[$k]){
            my $temp = $j-$sum;
            $var_index = $max[$k]-$temp;
            $var_num = $k;
            last;
          }
          else{
            $sum += ($max[$k]+1);
          }
        }
        my $max = ($#xor_array)/$vars;
        print OUT "v$var_num$var_index:=";
        if ($bit_array[$j] == 1){
          print OUT "TRUE";
        }
        else{
          print OUT "FALSE";
        }
      }
    }
    print OUT "]>";
  }
  print OUT "}\n";
}

sub var_units{
  #Changes the units of all values dependent on variables,
  #independently for each variable
  for (my $j = 1; $j <= $vars; $j++){
    #printf "What units should be used for variable %d? 10^x => x = ?\n", $j-1;
    #my $e_diff = <STDIN>;
    my $max_e = -100;
    for my $i(0..$#table){
      my $temp = $table[$i][$j];
      my $temp_e = 0;
      if ($temp =~ /e/){
        $temp =~ m/(.*)e(.*)/;
        $temp_e = $2;
      }
      else{
        while (abs($temp) < 1 && $temp != 0){
          $temp *= 10;
          $temp_e--;
        }
        while (abs($temp) >= 10){
          $temp /= 10;
          $temp_e++;
        }
      }
      if ($temp_e > $max_e){
        $max_e = $temp_e;
      }
    }
    for my $key(keys %avg_rate){
      if ($key =~ /$j$/){
        my $temp = $avg_rate{$key};
        $temp =~ m/(.*)e(.*)/;
        my $temp_e = $2;
        if ($temp_e > $max_e){
          $max_e = $temp_e;
        }
      }
    }
    my $e_diff = $max_e - 6;
    for my $i(0..$#table){
      $table[$i][$j] =~ m/(.*)e(.*)/;
      my $f = $1;
      my $e = $2;
      my $eout = $e - $e_diff;
      if($eout>=0){
        $eout = sprintf "+%02d", $eout;
      }
      else{
        $eout = sprintf "%03d", $eout;
      }
      $table[$i][$j] = sprintf "%.8fe%s", $f, $eout;
    }
    for my $i(0..$#rates){
      $rates[$i][$j-1] =~ m/(.*)e(.*)/;
      my $f = $1;
      my $e = $2;
      my $eout = $e - $e_diff;
      if($eout>=0){
        $eout = sprintf "+%02d", $eout;
      }
      else{
        $eout = sprintf "%03d", $eout;
      }
      $rates[$i][$j-1] = sprintf "%.8fe%s", $f, $eout;
    }
    for my $i(0..$#enab_bins){
      if ($enab_bins[$i][$j-1] =~ m/(.*)=(.*)/){
        my $temp = $1;
        my $num = $2;
        $enab_bins[$i][$j-1] = sprintf "%s=", $1;
        if ($num =~ /e/){
          $num =~ m/(.*)e(.*)/;
          my $f = $1;
          my $e = $2;
          my $eout = $e - $e_diff;
          if($eout>=0){
            $eout = sprintf "+%02d", $eout;
          }
          else{
            $eout = sprintf "%03d", $eout;
          }
          my $float = sprintf "%.8fe%s", $f, $eout;
          if ($temp eq ">"){
            $enab_bins[$i][$j-1] .= ftoi($float) + 1;
          }
          elsif ($temp eq "<"){
            $enab_bins[$i][$j-1] .= ftoi($float) - 1;
          }
        }
        else{
          if ($temp eq ">"){
            $enab_bins[$i][$j-1] .= (int $num*(10**-$e_diff)) + 1;
          }
          elsif ($temp eq "<"){
            $enab_bins[$i][$j-1] .= (int $num*(10**-$e_diff)) - 1;
          }
        }
      }
    }
    my $var_ind = $j-1;
    foreach my $key(keys %min_rate){
      if ($key =~ m/$var_ind$/){
        $min_rate{$key} =~ m/(.*)e(.*)/;
        my $f = $1;
        my $e = $2;
        my $eout = $e - $e_diff;
        if($eout>=0){
          $eout = sprintf "+%02d", $eout;
        }
        else{
          $eout = sprintf "%03d", $eout;
        }
        $min_rate{$key} = sprintf "%.8fe%s", $f, $eout;
      }
    }
    foreach my $key(keys %max_rate){
      if ($key =~ m/$var_ind$/){
        $max_rate{$key} =~ m/(.*)e(.*)/;
        my $f = $1;
        my $e = $2;
        my $eout = $e - $e_diff;
        if($eout>=0){
          $eout = sprintf "+%02d", $eout;
        }
        else{
          $eout = sprintf "%03d", $eout;
        }
        $max_rate{$key} = sprintf "%.8fe%s", $f, $eout;
      }
    }
    foreach my $key(keys %avg_rate){
      if ($key =~ m/$var_ind$/){
        $avg_rate{$key} =~ m/(.*)e(.*)/;
        my $f = $1;
        my $e = $2;
        my $eout = $e - $e_diff;
        if($eout>=0){
          $eout = sprintf "+%02d", $eout;
        }
        else{
          $eout = sprintf "%03d", $eout;
        }
        $avg_rate{$key} = sprintf "%.8fe%s", $f, $eout;
      }
    }
  }
}

sub time_units{
  #Changes all values dependent on time, according to the same scale
  #print "What time units are desired? 10^x => x = ?\n";
  #my $e_diff = <STDIN>;
  my $max_e = -100;
  for my $i(0..$#delay){
    my @temp = split /\s+/, $delay[$i];
    for my $j(0..$#temp){
      $temp[$j] =~ m/(.*)e(.*)/;
      my $flag = $1;
      my $temp_e = $2;
      if ($flag != 0.000){
        if ($temp_e > $max_e){
          $max_e = $temp_e;
        }
      }
    }
  }
  my $e_diff = $max_e;
  foreach my $key(keys %min_rate){
    $min_rate{$key} =~ m/(.*)e(.*)/;
    my $f = $1;
    my $e = $2;
    my $eout = $e + $e_diff;
    if($eout>=0){
      $eout = sprintf "+%02d", $eout;
    }
    else{
      $eout = sprintf "%03d", $eout;
    }
    $min_rate{$key} = sprintf "%.8fe%s", $f, $eout;
  }
  foreach my $key(keys %max_rate){
    $max_rate{$key} =~ m/(.*)e(.*)/;
    my $f = $1;
    my $e = $2;
    my $eout = $e + $e_diff;
    if($eout>=0){
        $eout = sprintf "+%02d", $eout;
      }
      else{
        $eout = sprintf "%03d", $eout;
      }
      $max_rate{$key} = sprintf "%.8fe%s", $f, $eout;
    }
  foreach my $key(keys %avg_rate){
    $avg_rate{$key} =~ m/(.*)e(.*)/;
    my $f = $1;
    my $e = $2;
    my $eout = $e + $e_diff;
    if($eout>=0){
      $eout = sprintf "+%02d", $eout;
    }
    else{
      $eout = sprintf "%03d", $eout;
    }
    $avg_rate{$key} = sprintf "%.8fe%s", $f, $eout;
  }
  for my $i(0..$#delay){
    my @temp_delay = split(/\s+/,$delay[$i]);
    for (my $j = 0; $j < $vars; $j++){
      $temp_delay[$j] =~ m/(.*)e(.*)/;
      my $f = $1;
      my $e = $2;
      my $eout = $e - $e_diff;
      if($eout>=0){
        $eout = sprintf "+%02d", $eout;
      }
      else{
        $eout = sprintf "%03d", $eout;
      }
      $temp_delay[$j] = sprintf "%.8fe%s", $f, $eout;
    }
    $delay[$i] = sprintf "%s %s", $temp_delay[0],$temp_delay[1];
  }
}

sub rate_units{
  #Changes all values dependent on time, according to the same scale
  #print "What time units are desired? 10^x => x = ?\n";
  #my $e_diff = <STDIN>;
  my $max_e = -100;
  for my $key(keys %avg_rate){
    $avg_rate{$key} =~ m/(.*)e(.*)/;
    my $flag = $1;
    my $temp_e = $2;
    if ($flag != 0.000){
      if ($temp_e > $max_e){
        $max_e = $temp_e;
      }
    }
  }
  my $e_diff = $max_e - 3;
  foreach my $key(keys %min_rate){
    $min_rate{$key} =~ m/(.*)e(.*)/;
    my $f = $1;
    my $e = $2;
    my $eout = $e - $e_diff;
    if($eout>=0){
      $eout = sprintf "+%02d", $eout;
    }
    else{
      $eout = sprintf "%03d", $eout;
    }
    $min_rate{$key} = sprintf "%.8fe%s", $f, $eout;
  }
  foreach my $key(keys %max_rate){
    $max_rate{$key} =~ m/(.*)e(.*)/;
    my $f = $1;
    my $e = $2;
    my $eout = $e - $e_diff;
    if($eout>=0){
        $eout = sprintf "+%02d", $eout;
      }
      else{
        $eout = sprintf "%03d", $eout;
      }
      $max_rate{$key} = sprintf "%.8fe%s", $f, $eout;
    }
  foreach my $key(keys %avg_rate){
    $avg_rate{$key} =~ m/(.*)e(.*)/;
    my $f = $1;
    my $e = $2;
    my $eout = $e - $e_diff;
    if($eout>=0){
      $eout = sprintf "+%02d", $eout;
    }
    else{
      $eout = sprintf "%03d", $eout;
    }
    $avg_rate{$key} = sprintf "%.8fe%s", $f, $eout;
  }
  for my $i(0..$#delay){
    my @temp_delay = split(/\s+/,$delay[$i]);
    for (my $j = 0; $j < $vars; $j++){
      $temp_delay[$j] =~ m/(.*)e(.*)/;
      my $f = $1;
      my $e = $2;
      my $eout = $e - $e_diff;
      if($eout>=0){
        $eout = sprintf "+%02d", $eout;
      }
      else{
        $eout = sprintf "%03d", $eout;
      }
      $temp_delay[$j] = sprintf "%.8fe%s", $f, $eout;
    }
    $delay[$i] = sprintf "%s %s", $temp_delay[0],$temp_delay[1];
  }
}
