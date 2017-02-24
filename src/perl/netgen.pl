#!/usr/bin/perl -w

while(<STDIN>) {
  chomp $_;
  if ($_) {
    push @a,[ split ];
  }
}


# Headers
@h = @{shift(@a)};

# Number of dimensions
$dims = int(@h/2);

# Count number of data points in each dimension
# Also find the max value for eachdimension
for ($d = 0; $d < $dims; $d++) {
  %seen = ();
  $maxs[$d] = $a[0][$d];
  $mins[$d] = $a[0][$d];
  for ($r = 0; $r <= $#a; $r++) {
    $seen{$a[$r][$d]} = 1;
    if ($a[$r][$d] > $maxs[$d]) {
      $maxs[$d] = $a[$r][$d];
    }
    if ($a[$r][$d] < $mins[$d]) {
      $mins[$d] = $a[$r][$d];
    }
  }
  $dpts[$d] = scalar(keys(%seen));
}

#for ($i = 0; $i < $dims; $i++) {
#  printf("$h[$i] datapoints: $dpts[$i]\n");
#  printf("$h[$i] min: $mins[$i]\n");
#  printf("$h[$i] max: $maxs[$i]\n");
#}

# dummy list.
print(".dummy ");
for ($r = 0; $r <= $#a; $r++) {
  # Continuous transitions in each region.
  for ($d = $dims; $d <= $#h; $d++) {
    print("$h[$d]_$r ");
  }
  # Discrete transitions in each region.
  for ($d = 0; $d < $dims; $d++) {
    if ($a[$r][$d] != $maxs[$d]) {
      print("$h[$d]u_$r ");
    }
    if ($a[$r][$d] != $mins[$d]) {
      print("$h[$d]d_$r ");
    }
  }
}
print("\n\n");

# Connect everything up
for ($r = 0; $r <= $#a; $r++) {
  print("# Region $r\n");
  # Continuous Arcs
  print("# Continuous Arcs\n");
  for ($c = $dims; $c <= $#h; $c++) {
    # Negative rate: place to transition arc
    if ($a[$r][$c] < 0) {
      print("$h[$c-$dims] $h[$c]_$r\n");
    }
    # Positive rate: transition to place arc
    else {
      print("$h[$c]_$r $h[$c-$dims]\n");
    }
  }
  # Discrete Arcs to transitions provided by this region.
  print("# Intra-region Arcs\n");
  for ($d = $dims-1; $d >= 0; $d--) {
    if ($a[$r][$d] != $mins[$d]) {
      print("p_$r $h[$d]d_$r #\@ [0,0]\n");
    }
    if ($a[$r][$d] != $maxs[$d]) {
      print("p_$r $h[$d]u_$r #\@ [0,0]\n");
    }
  }
  # Discrete Arcs to transitions provided by surrounding regionss.
  # I think this is correct for n-dimensions but I'm not positive.
  print("# Inter-region Arcs\n");
  $shift = 1;
  for ($d = $dims-1; $d >= 0; $d--) {
    if ($a[$r][$d] != $maxs[$d]) {
      print("$h[$d]d_", ($r+$shift), " p_$r #\@ [0,0]\n");
    }
    if ($a[$r][$d] != $mins[$d]) {
      print("$h[$d]u_", ($r-$shift), " p_$r #\@ [0,0]\n");
    }
    $shift *= $dpts[$d];
  }
  
  # Test Arcs
  print("# Test Arcs\n");
  # Discrete place to continuous transition arcs.
  for ($c = $dims; $c <= $#h; $c++) {
    print("p_$r $h[$c]_$r\n");
  }
  # Continuous place to discrete transition arcs.
  $shift = 1;
  for ($d = $dims-1; $d >= 0; $d--) {
    if ($a[$r][$d] != $mins[$d]) {
      $ubnd = int(($a[$r][$d] + $a[$r-$shift][$d])/2)-25;
      print("$h[$d] $h[$d]d_$r #\@ /0;$ubnd\\ [0,0]\n");
    }
    if ($a[$r][$d] != $maxs[$d]) {
      $lbnd = int(($a[$r][$d] + $a[$r+$shift][$d])/2)+25;
      print("$h[$d] $h[$d]u_$r #\@ /$lbnd;inf\\ [0,0]\n");
    }
    $shift *= $dpts[$d];
  }
  print("\n");
}
print("\n");

# Assign rates to continuous transitions
print("#\@.rates {");
for ($r = 0; $r <= $#a; $r++) {
  # Continuous transitions in each region.
  for ($c = $dims; $c <= $#h; $c++) {
    print("<$h[$c]_$r = ", abs($a[$r][$c]), ">");
  }
}
printf("}\n\n");


# Specify which things are continuous
print("#\@.continuous ");
# Continuous Places
for ($d = 0; $d < $dims; $d++) {
  print("$h[$d] ");
}
# Continuous transitions 
for ($r = 0; $r <= $#a; $r++) {
  for ($c = $dims; $c <= $#h; $c++) {
    print("$h[$c]_$r ");
  }
}
print("\n\n");

# Initial marking
print(".marking{p_0}\n");
print(".end\n");

print("\n");
