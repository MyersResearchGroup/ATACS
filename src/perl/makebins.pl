#!/usr/bin/perl

if ($#ARGV < 2){
    print "Usage ./makebins.pl <input data file> <output file> <# of bins>\n";
    exit(0);
}
open (IN, $ARGV[0]) or die "I cannot open the input file $ARGV[0]\n";
my @in = <IN>;
close IN;

open OUT, ">$ARGV[1]";

if ($#ARGV > 2){
  my @temp = split (/\s+/,$in[0]);
  my $vars = $#temp+1;
  $flag = 1;
  for (my $i = 0; $i < $vars; $i++){
    $bins[$i] = $ARGV[2+$i];
  }
}
else{
  $bins = $ARGV[2];
}

foreach my $line (@in){
  $line =~ s/\s+$//;
  my @stats = split (/\s+/,$line);
  for (my $i = 0; $i <= $#stats; $i++){
    $variables[$i]{$stats[$i]}++;
    @temp[$i] = $stats[$i];
  }
  push @table, [@temp];
}

for (my $i = 0; $i <= $#variables; $i++){
    my $k = $variables[$i];
    my %k = %$k;
    my @keys = sort {ourSort($a,$b)} keys %k;
    if ($#keys == 0){
      my $temp = @keys[0];
      for my $j(0..$#in){
        $keys[$j] = $temp;
      }
    }
    push @sort_list, [@keys];
}

if($flag == 1){
  for (my $i = 0; $i <= $#bins; $i++){
    $lines[$i] = $#in/$bins[$i];
  }
}
else{
  $lines = $#in/$bins;
}

for (my $i = 0; $i <= $#variables; $i++){
  if ($flag == 1){
    for (my $j = 0; $j <= $bins[$i]; $j++){
      my $index = $lines[$i]*($j);
      $divide[$bins[$i]-$j][$i] = $sort_list[$i][$index];
    }
  }
  else{
    for (my $j = 0; $j <= $bins; $j++){
      my $index = $lines*($j);
      $divide[$bins-$j][$i] = $sort_list[$i][$index];
    }
  }
}

shift @divide;
for $row(@divide){
  print OUT "@$row\n";
}

close OUT;


sub ourSort{
  my $a = shift;
  my $b = shift;
  $a =~ m/(.*)e(.*)/;
  my $f1 = $1;
  $b =~ m/(.*)e(.*)/;
  my $f2 = $1;
  if ($a =~ m/^[-]/ and $b =~ m/^[-]/){
    $a =~ s/^[-]//;
    $b =~ s/^[-]//;
    return ourSort($a,$b) * -1;
  }
  elsif ($a =~ m/^[-]/){
    return 1;
  }
  elsif ($b =~ m/^[-]/){
    return -1;
  }
  elsif ($a =~ m/e[-]/ and $b =~ m/e[-]/){
    $a =~ s/e-/e+/;
    $b =~ s/e-/e+/;
    return ourSort($a,$b) * -1;
  }
  elsif ($a =~ m/e[-]/){
    if ($f2 == 0.000){
      return -1;
    }
    else{
      return 1;
    }
  }
  elsif ($b =~ m/e[-]/){
    if ($f1 == 0.000){
      return 1;
    }
    else{
      return -1;
    }
  }
  else{
    $a =~ m/(.*)e[+](.*)/;
    my $f1 = $1;
    my $e1 = $2;
    $b =~ m/(.*)e[+](.*)/;
    my $f2 = $1;
    my $e2 = $2;
    if($f1 == 0.000000 and $f2 != 0.00){
      return 1;
    }
    elsif($f2 == 0.000000 and $f1 != 0.00){
      return -1;
    }
    elsif ($e1 < $e2){
      return 1;
    }
    elsif($e1 > $e2){
      return -1;
    }
    elsif ($f1 < $f2){
      return -1;
    }
    elsif($f1 > $f2){
      return 1;
    }
    else{
      return 0;
    }    
  }
}
