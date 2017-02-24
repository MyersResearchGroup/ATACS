#!/usr/bin/perl
#Written by Kevin Jones, Summer 2006

if ($#ARGV != 2){
    print "Usage ./altbinsort.pl <input data file> <bin file> <output file>\n";
    exit(0);
}
open (IN, $ARGV[0]) or die "I cannot open the input file $ARGV[0]\n";
my @in = <IN>;
close IN;

open (BIN, $ARGV[1]) or die "I cannot open the data file $ARGV[1]\n";
my @bin_in = <BIN>;
close BIN;

open OUT, ">$ARGV[2]";

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
  $bins[$i] = 0;
}

foreach my $line(@bin_in){
  my @temp = split(/\s+/,$line);
  for (my $i = 0; $i <= $#temp; $i++){
    if ($temp[$i] != ""){
      $bins[$i]++;
    }
  }
  push @divide, [@temp];
}

for (my $i = 1; $i <= $#variables; $i++){
  for (my $j = 0; $j < $#in; $j++){
    my $a = $table[$j][$i];
    for (my $k = 1; $k <= $bins[$i]; $k++){
      $c = $divide[$k-1][$i];
      if (compare($a,$c) < 0){
        if ($i == 2 && $j < 5){
      }
        $output[$j][$i-1] = $k-1;
        last;
      }
    }
    if ($output[$j][$i-1] eq ""){
      if ($bins[$i] > 0){
        $output[$j][$i-1] = $bins[$i]-1;
      }
      else{
        $output[$j][$i-1] = 0;
      }
    }
  }
}

#for (my $i=0; $i<$#table; $i++){
  #$table[$i][0] = $i;
#}

for my $i(0..$#output){
  print OUT "@{$table[$i]} ";
  print OUT "@{$output[$i]}\n";
}

close OUT;


sub compare{
  my $a = $_[0];
  my $b = $_[1];
  unless ($b =~ e){
    $a =~ m/(.*)e(.*)/;
    my $f = $1;
    my $e = $2;
    return -($b <=> ($f*(10**$e)));
  }
  $a =~ m/(.*)e(.*)/;
  my $f1 = $1;
  my $e1 = $2;
  $b =~ m/(.*)e(.*)/;
  my $f2 = $1;
  my $e2 = $2;
  if ($a =~ m/^[-]/ and $b =~ m/^[-]/){
    $a =~ s/^[-]//;
    $b =~ s/^[-]//;
    return compare($a,$b) * -1;
  }
  elsif ($a =~ m/^[-]/){
    return -1;
  }
  elsif ($b =~ m/^[-]/){
    return 1;
  }
  elsif ($a =~ m/e[-]/ and $b =~ m/e[-]/){
    $a =~ s/e-/e+/;
    $b =~ s/e-/e+/;
    if ($e1 != $e2){
      return compare($a,$b) * -1;
    }
    else{
      return compare($a,$b);
    }
  }
  elsif ($a =~ m/e[-]/){
    if ($f2 == 0.000000){
      return 1;
    }
    else{
      return -1;
    }
  }
  elsif ($b =~ m/e[-]/){
    if ($f1 == 0.000000){
      return -1;
    }
    else{
      return 1;
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
      return -1;
    }
    elsif($f2 == 0.000000 and $f1 != 0.00){
      return 1;
    }
    elsif ($e1 < $e2){
      return -1;
    }
    elsif($e1 > $e2){
      return 1;
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
