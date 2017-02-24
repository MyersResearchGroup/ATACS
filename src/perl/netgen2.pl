#!/usr/bin/perl -w

#Modified 10/10/03 to work with new splitting method.

print("# Data used to generate graph:\n");
while(<STDIN>) {
  chomp $_;
  print("# " . $_ . "\n");
  if ($_) {
    push @a,[ split ];
  }
}

print("\n");

# Headers
@h = @{shift(@a)};

# Number of dimensions
$dims = int(@h/3);

# Count number of data points in each dimension
# Also find the max value for each dimension
for ($d = 0; $d < $dims*2; $d+=2) {
  %seen = ();
  $maxs[$d/2] = $a[0][$d+1];
  $mins[$d/2] = $a[0][$d];
  for ($r = 0; $r <= $#a; $r++) {
    $seen{$a[$r][$d]} = 1;
    if ($a[$r][$d+1] > $maxs[$d/2]) {
      $maxs[$d/2] = $a[$r][$d+1];
    }
    if ($a[$r][$d] < $mins[$d/2]) {
      $mins[$d/2] = $a[$r][$d];
    }
  }
  $dpts[$d/2] = scalar(keys(%seen));
}
#for ($i = 0; $i < $dims; $i++) {
#  $var = substr($h[$i*2], 0, -1);
#  printf("$var datapoints: $dpts[$i]\n");
#  printf("$var min: $mins[$i]\n");
#  printf("$var max: $maxs[$i]\n");
#}

# dummy list.
print(".dummy ");
for ($r = 0; $r <= $#a; $r++) {
  # Continuous transitions in each region.
  for ($d = $dims*2; $d <= $#h; $d++) {
    print("$h[$d]_$r ");
  }
}
# Discrete transitions in each dimension.
for ($d = 0; $d < $dims; $d++) {
  for ($r = 0; $r < $dpts[$d]; $r++) {
    if ($r != 0) {
      print("$h[$d*2]_$r ");
    }
    if ($r != $dpts[$d]-1) {
      print("$h[$d*2+1]_$r ");
    }
  }
}
print("\n\n");

# Continuous place to discrete transition test arcs.
print("# Test Arcs - Continuous Places to Discrete Transitions\n");
$total = 1;
for ($d = 0; $d < $dims; $d++) {
  $total = $total * $dpts[$d];
}
for ($d = 0; $d < $dims; $d++) {
  $contPlace = substr($h[$d*2], 0, -1);
  # Discrete transitions in each dimension.  
  $total = $total / $dpts[$d];
  for ($r = 0; $r < $dpts[$d]; $r++) {
    if ($r != 0) {
      $lower =$a[$total*($r-1)][$d*2];
      $upper =$a[$total*($r-1)][$d*2+1];
      print("$contPlace $h[$d*2]_$r #\@ /$lower;$upper\\ [0,0]\n");
    }
    if ($r != $dpts[$d]-1) {
      $lower =$a[$total*($r+1)][$d*2];
      $upper =$a[$total*($r+1)][$d*2+1];
      print("$contPlace $h[$d*2+1]_$r #\@ /$lower;$upper\\ [0,0]\n");
    }
  }
}
print("\n");

# Connections between discrete places and transitions
# along each axis.
print("# Arcs along each dimension's axis\n");
for ($d = 0; $d < $dims; $d++) {
  for ($r = $dpts[$d]-1; $r >= 0; $r--) {
    $p = substr($h[$d*2], 0, -1);
    if ($r != 0) {
      print("$p" . "_$r $h[$d*2]_$r #\@ [0,0]\n");
      print("$h[$d*2]_$r $p" . "_" . ($r-1). " #\@ [0,0]\n");
    }
  }
  for ($r = 0; $r < $dpts[$d]; $r++) {
    $p = substr($h[$d*2+1], 0, -1);
    if ($r != $dpts[$d]-1) {
      print("$p" . "_$r $h[$d*2+1]_$r #\@ [0,0]\n");
      print("$h[$d*2+1]_$r $p" . "_" . ($r+1). " #\@ [0,0]\n");
    }
  }
}

printf("\n");

#   # Discrete Arcs to transitions provided by this region.
#   print("# Intra-region Arcs\n");
#   for ($d = $dims*2-1; $d >= 0; $d-=2) {
#     if ($a[$r][$d] != $maxs[$d/2] && $a[$r][$dims*2+$d/2] > 0) {
#        print("p_$r $h[$d]_$r #\@ [0,0]\n");
#     }
#     if ($a[$r][$d-1] != $mins[$d/2] && $a[$r][$dims*2+$d/2] < 0) {
#        print("p_$r $h[$d-1]_$r #\@ [0,0]\n");
#     }
#   }
#   # Discrete Arcs to transitions provided by surrounding regions.
#   # I think this is correct for n-dimensions but I'm not positive.
#   print("# Inter-region Arcs\n");
#   $shift = 1;
#   for ($d = $dims*2-1; $d >= 0; $d-=2) {
#     if ($a[$r][$d] != $maxs[$d/2] && $a[$r+$shift][$dims*2+$d/2] < 0) {
#       print("$h[$d-1]_", ($r+$shift), " p_$r #\@ [0,0]\n");
#     }
#     if ($a[$r][$d-1] != $mins[$d/2] && $a[$r-$shift][$dims*2+$d/2] > 0) {
#       print("$h[$d]_", ($r-$shift), " p_$r #\@ [0,0]\n");
#     }
#     $shift *= $dpts[$d/2];
#   }



# Connect everything up
for ($r = 0; $r <= $#a; $r++) {
  print("# Region $r\n");
  # Continuous Arcs
  print("# Continuous Arcs\n");
  for ($c = $dims*2; $c <= $#h; $c++) {
    # Negative rate: place to transition arc
    $contPlace = substr($h[$c], 1);
    if ($a[$r][$c] < 0) {
      print("$contPlace $h[$c]_$r\n");
    }
    # Positive rate: transition to place arc
    else {
      print("$h[$c]_$r $contPlace\n");
    }
  }
  
  # Test Arcs
  print("# Test Arcs\n");
  # Discrete place to continuous transition arcs.
  $divider = 1;
  for ($d = 0; $d < $dims; $d++) {
    $divider = $divider * $dpts[$d];
  }
  $index = $r;
  for ($d = 0; $d < $dims; $d++) {
    $divider = int($divider / $dpts[$d]);
    $pos = int($index/$divider);
    for ($d1 = 0; $d1 < $dims; $d1++) {
      print(substr($h[$d*2],0,-1) . "_$pos $h[$dims*2+$d1]_$r\n");
    }
    $index = $index % $divider;
  }
  print("\n");
}
print("\n");

# Assign rates to continuous transitions
print("#\@.rates {");
for ($r = 0; $r <= $#a; $r++) {
  # Continuous transitions in each region.
  for ($c = $dims*2; $c <= $#h; $c++) {
    print("<$h[$c]_$r = ", abs($a[$r][$c]), ">");
  }
}
printf("}\n\n");


# Specify which things are continuous
print("#\@.continuous ");
# Continuous Places
for ($d = 0; $d < $dims*2; $d+=2) {
  print(substr($h[$d], 0, -1));
  print(" ");
}
# Continuous transitions 
for ($r = 0; $r <= $#a; $r++) {
  for ($c = $dims*2; $c <= $#h; $c++) {
    print("$h[$c]_$r ");
  }
}
print("\n\n");

# Initial marking
print(".marking{ ");
for ($d = 0; $d < $dims; $d++) {
  print(substr($h[$d*2], 0, -1) . "_0 ");
}
print("}\n");
print(".end\n");

print("\n");
