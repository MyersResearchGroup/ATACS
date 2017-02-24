#!/usr/bin/perl -w

print("-- Data used to generate automaton:\n");
while(<STDIN>) {
  chomp $_;
  print("-- " . $_ . "\n");
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


print("var\n");
for ($c = $dims*2; $c <= $#h; $c++) {
  print("  ", substr($h[$c], 1), ": analog;\n");
}

print ("\n");
print ("automaton a1\n");
print ("synclabs:;\n");

# Initial conditions
print ("initially l0 & ");
$r = 0;
$shift = 1;
for ($d = $dims*2-1; $d >= 0; $d-=2) {
  $contPlace = substr($h[$d], 0, -1);
  print("$contPlace=1");
#  if ($a[$r][$d-1] != $mins[$d/2]) {
#    print("$contPlace>=$a[$r-$shift][$d]");
#  }
#  else {
#    print("$contPlace>=0");
#  }
#  print(" & ");
#  if ($a[$r][$d] != $maxs[$d/2]) {
#    print("$contPlace<=$a[$r+$shift][$d-1]");
#  }
#  else {
#    print("$contPlace<=$a[$r][$d]");
#  }
#  $shift *= $dpts[$d/2];
  if ($d > 1) {
    print(" & ");
  }
}
print(";\n\n");


#Special below zero locations
$skip = 1;
for ($d1 = 0; $d1 < $dims; $d1++) {
  for ($r = 0; $r < $dpts[($d1+1)%$dims]*$skip; $r+=$skip) {
    print("loc ld$d1", "r$r: while ");
    # Conditions in special locations
    $shift = 1;
    for ($d = $dims*2-1; $d >= 0; $d-=2) {
      $contPlace = substr($h[$d], 0, -1);
      if ($d == $d1*2+1) {
        print("$contPlace<=0");
      }
      else {
        if ($a[$r][$d-1] != $mins[$d/2]) {
          print("$contPlace>=$a[$r-$shift][$d]");
        }
        else {
          print("$contPlace>0");
        }
        if ($a[$r][$d] != $maxs[$d/2]) {
          print(" & $contPlace<=$a[$r+$shift][$d-1]");
        }
        #else {
        #  print("$contPlace<=$a[$r][$d]");
        #}
      }
      $shift *= $dpts[$d/2];
      if ($d > 1) {
        print(" & ");
      }      
    }

    # Rates in special locations
    print(" wait {");
    for ($c = $dims*2; $c <= $#h; $c++) {
      if ($c == $d1+$dims*2 && $a[$r][$c] < 0) {
        print("$h[$c]=0");
      }
      else { 
        print("$h[$c]=$a[$r][$c]");
      }
      if ($c < $#h) {
        print(", ");
      }
    }
    print("}\n");

    # Possible transitions to take.
    $shift = 1;
    for ($d = $dims*2-1; $d >= 0; $d-=2) {
      $contPlace = substr($h[$d], 0, -1);
      if ($d == $d1*2+1) {
        print("  when $contPlace>=0 goto l$r;\n");
      }
      else {
        if ($a[$r][$d-1] != $mins[$d/2]) {
          $loc = $r-$shift;
          print("  when $contPlace=$a[$loc][$d] goto ld$d1", "r$loc;\n");
        }
        if ($a[$r][$d] != $maxs[$d/2]) {
          $loc = $r+$shift;
          print("  when $contPlace=$a[$loc][$d-1] goto ld$d1", "r$loc;\n");
        }
      }
      $shift *= $dpts[$d/2];
    }  
  
    print("\n");
  }

  $skip *= $dpts[($d1+1)%$dims];
}

# Other regular locations corresponding to table data.
for ($r = 0; $r <= $#a; $r++) {
  print ("loc l$r: while ");

  # Conditions to be in this location.
  $shift = 1;
  for ($d = $dims*2-1; $d >= 0; $d-=2) {
    $contPlace = substr($h[$d], 0, -1);
    if ($a[$r][$d-1] != $mins[$d/2]) {
      print("$contPlace>=$a[$r-$shift][$d]");
    }
    else {
      print("$contPlace>=0");
    }
    if ($a[$r][$d] != $maxs[$d/2]) {
      print(" & $contPlace<=$a[$r+$shift][$d-1]");
    }
    #else {
    #  print("$contPlace<=$a[$r][$d]");
    #}
    $shift *= $dpts[$d/2];
    if ($d > 1) {
      print(" & ");
    }
  }
  # Rates while in this location
  print(" wait {");
  for ($c = $dims*2; $c <= $#h; $c++) {
    print("$h[$c]=$a[$r][$c]");
    if ($c < $#h) {
      print(", ");
    }
  }
  print("}\n");

  # Possible transitions to take.
  $shift = 1;
  for ($d = $dims*2-1; $d >= 0; $d-=2) {
    $contPlace = substr($h[$d], 0, -1);
    if ($a[$r][$d-1] != $mins[$d/2]) {
      $loc = $r-$shift;
      print("  when $contPlace=$a[$loc][$d] goto l$loc;\n");
    }
    else {
      $dn = ($d-1)/2;
      print("  when $contPlace=0 goto ld$dn","r$r;\n");
    }
    if ($a[$r][$d] != $maxs[$d/2]) {
      $loc = $r+$shift;
      print("  when $contPlace=$a[$loc][$d-1] goto l$loc;\n");
    }
    $shift *= $dpts[$d/2];
  }  

  print("\n");
}

print("end\n");

