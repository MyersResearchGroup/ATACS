#!/usr/bin/perl

if ($#ARGV > 1 || $ARGV[0] eq ""){
  print "Usage ./postproc.pl (-t) <input file>\n";
  exit 0;
}

if ($ARGV[0] =~ /^-/){
  if ($ARGV[0] =~ /t/){
    $trans_flag = 1;
    open (IN, $ARGV[1]) or die "I cannot open input file $ARGV[1]\n";
    @in = <IN>;
    close IN;
    $outputF = $ARGV[1];
    $outputF =~ s/\.g$/_pp.g/;
    #print "Output file: $outputF\n";
    open OUT, ">$outputF";
  }
  elsif ($ARGV[0] =~ /^-h/){
    print "-f flag adds additional transitions\n";
    exit 0;
  }
  else{
    print "Invalid flag\n";
    exit 0;
  }
}
else{
  open (IN, $ARGV[0]) or die "I cannot open input file $ARGV[0]\n";
  @in = <IN>;
  close IN;
  $outputF = $ARGV[0];
  $outputF =~ s/\.g$/_pp.g/;
  #print "Output file: $outputF\n";
  open OUT, ">$outputF";
}
  

foreach my $line(@in){ #For each line of input
  $line =~ s/\s+$//;
  if ($line =~ m/^\#@\.rate_assignments \{(.*)\}/){
    $rate_line = $1; #Save the rate assignments
  }
  elsif ($line =~ m/^\#@\.enablings \{(.*)\}/){
    $enab_line = $1; #Save the enabling conditions
  }
  elsif ($line =~ /variables/){
    my @temp = split / /, $line; #Create an array of the variable names
    @vars = @temp; #Saves the array
    shift @vars; #Removes the line name (#@.variables)
    $vars = $#temp; #Saves the number of variables
  }
  elsif ($line =~ /boolean_assignments \{(.*)\}/){
    $bool_line = $1; #Saves the boolean assignments
  }
}

#print "rateLine:$rate_line\n";
$rate_line =~ s/<//g;
@rate_array = split />/, $rate_line;
#print "rateArray:@rate_array\n";
for my $i(0..$#rate_array){
  print "rateArray[i]:$rate_array[$i]\n";
  $rate_array[$i] =~ /(.*)=\[v/;
  $transition[$i] = $1; #Creates an array of the transitions
  #print "xition:$1\n";
  $rate_array[$i] =~ /\[(.*)\]$/;
  my $line = $1;
  print "line: $line\n";
  for my $j(0..$vars-1){ #Creates arrays for the minimum and maximum
                         #rates for each transition
    if ($j < $vars-1){
      $line =~ /:=\[(.*),(.*)\];/;
      $min_rate[$i][$j] = $1;
      $max_rate[$i][$j] = $2;
      print "$i,$j: $1, $2\n";
      $line =~ s/(.*);//;
    }
    else{
      $line =~ /\[(.*),(.*)\]/;
      $min_rate[$i][$j] = $1;
      $max_rate[$i][$j] = $2;
      print "$i,$j: $1, $2\n";
    }
  }
}

print "enabLine:$enab_line\n";
$enab_line =~ s/<//;
@enab_array = split /></, $enab_line;
print "enabArray:@enab_array\n";
for my $i(0..$#enab_array){
  print "enab_array[i]: $enab_array[$i]\n";
  $enab_array[$i] =~ /=\[(.*)\]/;
  my $line = $1;
  #print "Line: $line\n";
  my $temp_line = $line;
  for my $j(0..$vars-1){ #Saves the enabling conditions for each transition
    if ($j < $vars-1){
      if ($temp_line =~ /v\d+([>,<]=-?[0-9]+)&/){ 
        my $temp = $1;
        $temp =~ s/\s+$//;
        $enab[$i][$j] = $temp;
        print "enab[$i][$j]: $temp\n";
      }
      #else{
      #  $temp_line =~ /v\d+([)/;
      #  my $temp = $1;
      #  $temp =~ s/\s+$//;
      #  $enab[$i][$j] = $temp;
      #}
      $temp_line =~ s/(.*)&//;
    }
    else{
      $temp_line =~ /v\d+(.*)/;
      my $temp = $1;
      $temp =~ s/\s+$//;
      $enab[$i][$j] = $temp;
      print "enab[$i][$j]: $temp\n";
    }
  }
}

@bool_array = split /</, $bool_line;
for my $i(0..$#bool_array){ #Creates an array of boolean assignments
  $bool_array[$i] =~ /(.*)=\[(.*)>/;
  $bool_hash{$1} = $2;
}

for my $i(0..$#transition){
  $transition[$i] =~ /tp(.*)p(.*)/;
  $fromplace[$i] = sprintf "p%d", $1; #Saves the place from which
                                      #each transition goes
  $toplace[$i] = sprintf "p%d", $2; #Saves the place names according
                                    #to the place from which they are reached
}

for my $i(0..$#toplace){
  my $rate_flag = 0;
  my $temp = $toplace[$i];
  $temp =~ s/p//;
  $new_place[$i] = sprintf "cp%s", $temp; #Creates copies of relevant
                                   #place names
  my $cp_flag = 0;
  for my $j(0..$vars){
    if ($min_rate[$i][$j] != $max_rate[$i][$j]){
      $cp_flag = 1;
      last;
    }
  }
  if ($cp_flag != 1){ #Creates transitions relevant for each copied place
    #Note: copied places not used if minimum rate equals maximum rate
    $new_transition{$i} = sprintf "t%sto%s", $toplace[$i], $new_place[$i];
    for my $j(0..$#fromplace){
      if ($fromplace[$j] eq $toplace[$i]){
        $cp_transition{$i} = sprintf "t%sto%s", $new_place[$i], $toplace[$j];
      }
      if ($trans_flag == 1 && $tocp_transition{$i} eq ""){
        $tocp_transition{$i} = sprintf "t%sto%s", $fromplace[$i], $new_place[$i];
      }
    }
  }
  for my $j(0..$vars){
    if ($min_rate[$i][$j] < 0 && $max_rate[$i][$j] > 0){
      $rate_flag = 1;
    }
  }
  if ($rate_flag = 1){
    $ret_transition{$i} = sprintf "t%sto%s", $new_place[$i], $toplace[$i];
    #Create alternate transitions if the rate spans zero
  }
}

foreach my $line(@in){ #Prints the new information to the temp file
  #See the comprehensive documentation for how the net changes and why
  if ($line =~ /^.outputs/){ #Prints the Boolean variables
    $line =~ s/\s+$//;
    print OUT "$line cp\n";
    #Adds a Boolean variable to state whether the place is the minimum
    #rate or the maximum rate
  }
  elsif ($line =~ /^.dummy/){
    $line =~ s/\s+$//;
    print OUT "$line "; #Prints the .marking line with all the new
                        #transitions added
    foreach my $key(keys %new_transition){
      my $row = $new_transition{$key};
      if ($flag{$row} != 1 && $row !~ /to$/){
        print OUT "$row ";
        $flag{$row} = 1;
      }
    }
    foreach my $key(%cp_transition){
      my $row = $cp_transition{$key};
      if ($flag{$row} != 1 && $row !~ /to$/){
        print OUT "$row ";
        $flag{$row} = 1;
      }
    }
    foreach my $key(keys %ret_transition){
      my $temp = $ret_transition{$key};
      if ($flag{$temp} != 1 && $temp !~ /to$/){
        print OUT "$temp ";
        $flag{$ret_transition{$key}} = 1;
      }
    }
    if ($trans_flag == 1){
      foreach my $key(keys %tocp_transition){
        my $row = $tocp_transition{$key};
        if ($flag{$row} != 1 && $row !~ /to$/){
          print OUT "$row ";
          $flag{$row} = 1;
        }
      }
    }
    print OUT "\n";
  }
  elsif ($line =~ /init_state/){
    $line =~ s/\]/0\]/; #Adds the cp bit to the initial state
    print OUT "$line\n";
  }
  elsif ($line =~ /^t/){ #Adds the new transitions and places to the
                         #Petri net
    print OUT "$line\n"; #While copying the old ones
    my @temp = split / /, $line;
    for my $i(0..$#toplace){
      if ($toplace[$i] eq $temp[1]){
        $index = $i;
        last;
      }
    }
    if ($new_transition{$index} ne ""){
      if ($flag{$new_transition{$index}} != 3){
        print OUT "$toplace[$index] $new_transition{$index} #@[0,inf]\n";
        print OUT "$new_transition{$index} $new_place[$index]\n";
        $flag{$new_transition{$index}} = 3;
      }
      if ($ret_transition{$index} ne "" && $flag{$ret_transition{$index}} != 3){
        print OUT "$new_place[$index] $ret_transition{$index} #@[0,inf]\n";
        print OUT "$ret_transition{$index} $toplace[$index]\n";
        $flag{$ret_transition{$index}} = 3;
      }
      if ($flag{$cp_transition{$index}} != 3){
        for my $i(0..$#fromplace){
          if ($fromplace[$i] eq $toplace[$index]){
            print OUT "$new_place[$index] $cp_transition{$index} #@[0,0]\n";
            print OUT "$cp_transition{$index} $toplace[$i]\n";
            $flag{$cp_transition{$index}} = 3;
          }
        }
      }
      if ($trans_flag == 1){
        if ($tocp_transition{$index} ne "" && $flag{$tocp_transition{$index}} != 3){
          print OUT "$fromplace[$index] $tocp_transition{$index} #@ [0,0]\n";
          print OUT "$tocp_transition{$index} $new_place[$index]\n";
          $flag{$tocp_transition{$index}} = 3;
        }
      }
    }
  }
  elsif ($line =~ /^\#@\.enablings/){
    print OUT "#@.enablings {"; #Reprints the enabling conditions
    for my $i(0..$#transition){
      print OUT "<$transition[$i]=[";
      my $enab_flag = 0;
      for my $j(0..$vars-1){
        if ($enab_flag == 0 && $enab[$i][$j] ne ""){
          #print "$i, $j, $enab[$i][$j]\n";
          print OUT "$vars[$j]$enab[$i][$j]";
          $enab_flag = 1;
        }
        elsif ($enab_flag == 1 && $enab[$i][$j] ne ""){
          print OUT "&$vars[$j]$enab[$i][$j]";
        }
      }
      print OUT "]>";
    }
    foreach my $key(keys %cp_transition){ #And adds the new ones
      if ($cp_transition{$key} =~ /to$/){
        next;
      }
      print OUT "<$cp_transition{$key}=[";
      my $enab_flag = 0;
      for my $j(0..$vars-1){
        if ($enab_flag == 0 && $enab[$key+1][$j] ne ""){
          print OUT "$vars[$j]$enab[$key+1][$j]";
          $enab_flag = 1;
        }
        elsif ($enab_flag == 1 && $enab[$key+1][$j] ne ""){
          print OUT "&$vars[$j]$enab[$key+1][$j]";
        }
      }
      print OUT "]>";
    }
    if ($trans_flag == 1){
      foreach my $key(keys %tocp_transition){
        if ($tocp_transition{$key} =~ /to$/){
          next;
        }
        print OUT "<$tocp_transition{$key}=[";
        my $enab_flag = 0;
        for my $j(0..$vars-1){
          if ($enab_flag == 0 && $enab[$key][$j] ne ""){
            print OUT "$vars[$j]$enab[$key][$j]";
            $enab_flag = 1;
          }
          elsif ($enab_flag == 1 && $enab[$key][$j] ne ""){
            print OUT "&$vars[$j]$enab[$key][$j]";
          }
        }
        print OUT "]>";
      }
    }
    print OUT "}\n";
  }
  elsif ($line =~ /^\#@.rate_assignments/){
    print OUT "#@.rate_assignments {";
    for my $i(0..$#transition){ #Reprints the rate assignments
      print OUT "<$transition[$i]=[";
      for my $j(0..$vars-1){
        if ($j == 0){
          print OUT "$vars[$j]:=$min_rate[$i][$j]";
        }
        else{
          print OUT ";$vars[$j]:=$min_rate[$i][$j]";
        }
      }
      print OUT "]>";
    }
    foreach my $key(keys %new_transition){ #And adds the new ones
      print OUT "<$new_transition{$key}=[";
      for my $j(0..$vars-1){
        if ($j == 0){
          print OUT "$vars[$j]:=$max_rate[$key][$j]";
        }
        else{
          print OUT ";$vars[$j]:=$max_rate[$key][$j]";
        }
      }
      print OUT "]>";
    }
    foreach my $key(keys %ret_transition){
      print OUT "<$ret_transition{$key}=[";
      for my $i(0..$transition){
        if ($transition[$i] eq $key){
          $index = $i;
          last;
        }
      }
      for my $j(0..$vars-1){
        if ($j == 0){
          print OUT "$vars[$j]:=$min_rate[$index][$j]";
        }
        else{
          print OUT ";$vars[$j]:=$min_rate[$index][$j]";
        }
      }
      print OUT "]>";
    }
    foreach my $key(keys %cp_transition){
      if ($cp_transition{$key} =~ /to$/){
        next;
      }
      print OUT "<$cp_transition{$key}=[";
      for my $j(0..$vars-1){
        if ($j == 0){
          print OUT "$vars[$j]:=$min_rate[$key+1][$j]";
        }
        else{
          print OUT ";$vars[$j]:=$min_rate[$key+1][$j]";
        }
      }
      print OUT "]>";
    }
    if ($trans_flag == 1){
      foreach my $key(keys %tocp_transition){
        if ($tocp_transition{$key} =~ /to$/){
          next;
        }
        print OUT "<$tocp_transition{$key}=[";
        for my $j(0..$vars-1){
          if ($j == 0){
            print OUT "$vars[$j]:=$min_rate[$key][$j]";
          }
          else{
            print OUT ";$vars[$j]:=$min_rate[$key][$j]";
          }
        }
        print OUT "]>";
      }
    }
    print OUT "}\n";
  }
  elsif ($line =~ /^\#@.delay_assignments/){ #Prints the delay assignments
    print OUT "#@.delay_assignments {";
    foreach my $key(keys %flag){
      $flag{$key} = 0;
    }
    foreach my $row(@transition){
      print OUT "<$row=[0,0]>";
    }
    foreach my $key(keys %new_transition){ #And adds the new ones
      my $row = $new_transition{$key};
      if ($flag{$row} != 1){
        print OUT "<$row=[0,inf]>";
        $flag{$row} = 1;
      }
    }
    foreach my $key(keys %ret_transition){
      if ($flag{$ret_transition{$key}} != 1){
        print OUT "<$ret_transition{$key}=[0,inf]>";
        $flag{$ret_transition{$key}} = 1;
      }
    }
    foreach my $key(keys %cp_transition){
      my $row = $cp_transition{$key};
      if ($row !~ /to$/ && $flag{$row} != 1){
        print OUT "<$row=[0,0]>";
        $flag{$row} = 1;
      }
    }
    if ($trans_flag == 1){
      foreach my $key(keys %tocp_transition){
        my $row = $tocp_transition{$key};
        if ($row !~ /to$/ && $flag{$row} != 1){
          print OUT "<$row=[0,0]>";
          $flag{$row} = 1;
        }
      }
    }
    print OUT "}\n";
  }
  elsif ($line =~ /^\#@.boolean_assignments/){
    print OUT "#@.boolean_assignments {";
    foreach my $row(@transition){ #Reprints the boolean assignments
      if ($flag{$row} != 2){
        my $row_temp = $row;
        $row_temp =~ s/^<//;
        print OUT "<$row=[$bool_hash{$row_temp}>";
        $flag{$row} = 2;
      }
    }
    foreach my $key(keys %new_transition){ #And adds the new ones
      my $row = $new_transition{$key};
      if ($flag{$row} != 2){
        print OUT "<$row=[cp:=TRUE]>";
        $flag{$row} = 2;
      }
    }
    foreach my $key(keys %ret_transition){
      if ($flag{$ret_transition{$key}} != 2){
        print OUT "<$ret_transition{$key}=[cp:=FALSE]>";
        $flag{$ret_transition{$key}} = 2;
      }
    }
    foreach my $key(keys %cp_transition){
      my $row = $cp_transition{$key};
      if ($flag{$row} != 2){
        foreach my $new_key(keys %bool_hash){
          my $row_temp = $row;
          $row_temp =~ s/c//;
          if ($row_temp eq $new_key){
            my $temp = $bool_hash{$new_key};
            $temp =~ s/\]$//;
            print OUT "<$row=[$temp;cp:=FALSE]>";
            last;
          }
        }
      }
    }
    if ($trans_flag == 1){
      foreach my $key(keys %tocp_transition){
        my $row = $tocp_transition{$key};
        if ($flag{$row} != 2){
          foreach my $new_key(keys %bool_hash){
            my $row_temp = $row;
            $row_temp =~ s/c//;
            if ($row_temp eq $new_key){
              my $temp = $bool_hash{$new_key};
              $temp =~ s/\]$//;
              print OUT "<$row=[$temp;cp:=TRUE]>";
              last;
            }
          }
        }
      }
    }
    print OUT "}\n";
  }
  else{
    print OUT "$line\n";
  }
}

close OUT;

# if ($trans_flag == 1){
#   system "cp TMPFILE $ARGV[1]"; #Copies the temp file to the original file
# }
# else{
#   system "cp TMPFILE $ARGV[0]"; #Copies the temp file to the original file
# }

# system "rm TMPFILE"; #Removes the temp file
