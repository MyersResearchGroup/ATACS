#!/usr/bin/perl -w

# 12/8/05 - The netgen.pl script was modified to create the new lhpn
# structure resulting in this file.
#
# Expected Data file format:
#
# xd xu yd yu dx dy
# 0  5  0  5  1  2
# 0  5  6  10 2  4
# .  .  .  .  .  .
#
# Generated g file has these properties:
# - No continuous places or transitions, just continuous variables.
# - No invariants.
# - Discrete transitions are forced to fire using a timing of [0,0] so
#   when the enabling condition becomes satisfied, the transition will
#   fire immediately.
# - The upward discrete transitions are enabled when the neighboring
#   lower bound is reached.
# - The downward discrete transitions are enabled when the neighboring
#   upper bound is reached.
#
# TODO: prune out discrete transitios that will never be able to
# fire.  For example, if we are in a region where the rate is
# increasing for a particular dimension, do not put in the downward
# discrete transitions in that dimension.
#
# Command-line options:
#  -i    turns on the printing of the invariants. Otherwise,
#        invariants are not printed.

use Getopt::Std;

$opt_i = 0;
&getopts("i");

print("# Data used to generate graph:\n");
while(<STDIN>) {
  chomp $_;
  print("# " . $_ . "\n");
  if ($_) {
    push @a,[ split ];
  }
}

# Headers
@h = @{shift(@a)};

# Number of dimensions
$dims = int(@h/3);

# Count number of data points in each dimension
# Also find the max value for eachdimension
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

# Test code to print min/max/# of datapoints in each dimension.
#for ($i = 0; $i < $dims; $i++) {
#  $var = substr($h[$i*2], 0, -1);
#  printf("$var datapoints: $dpts[$i]\n");
#  printf("$var min: $mins[$i]\n");
#  printf("$var max: $maxs[$i]\n");
#}

# dummy list.
print(".dummy ");
# Discrete transitions in each region.
for ($d = 0; $d < $dims; $d++) {
  for ($r = 0; $r <= $#a; $r++) {
    if ($a[$r][$d*2] != $mins[$d]) {
      print("$h[$d*2]_$r ");
    }
    if ($a[$r][$d*2+1] != $maxs[$d]) {
      print("$h[$d*2+1]_$r ");
    }
  }
}
print("\n\n");

# 12/8/05 - Variables
print("#\@.variables ");
for ($d = 0; $d < $dims; $d++) {
  $var = substr($h[$d*2], 0, -1);
  print("$var ");
}
print("\n\n");

# Connect everything up
for ($r = 0; $r <= $#a; $r++) {
  print("# Region $r\n");
  # Discrete Arcs to transitions provided by this region.
  print("# Intra-region Arcs\n");
  for ($d = $dims-1; $d >= 0; $d--) {
    if ($a[$r][$d*2] != $mins[$d]) {
      print("p_$r $h[$d*2]_$r #@ [0,0]\n");
    }
    if ($a[$r][$d*2+1] != $maxs[$d]) {
      print("p_$r $h[$d*2+1]_$r #@ [0,0]\n");
    }
  }
  # Discrete Arcs to transitions provided by surrounding regionss.
  # I think this is correct for n-dimensions but I'm not positive.
  print("# Inter-region Arcs\n");
  $shift = 1;
  for ($d = $dims-1; $d >= 0; $d--) {
    if ($a[$r][$d*2+1] != $maxs[$d]) {
      print("$h[$d*2]_", ($r+$shift), " p_$r #@ [0,0]\n");
    }
    if ($a[$r][$d*2] != $mins[$d]) {
      print("$h[$d*2+1]_", ($r-$shift), " p_$r #@ [0,0]\n");
    }
    $shift *= $dpts[$d];
  }
}
print("\n");

# 12/8/05 - Assign invariants to discreten places.
# Don't need invariants because the timers are being used to force
# firing of discrete transitions when their enabling condition
# is satisifed.
# However, if being used with David's analyzer, timers are ignored so
# invariants are necessary.  Thus the -i flag can turn them on or off.

if ($opt_i) {
  print("#\@.invariants {");
  for ($r = 0; $r <= $#a; $r++) {
    $shift = 1;
    print("<p_$r=[");
    $addsemi = 0;
    $shift = 1;
    for ($d = $dims-1; $d >= 0; $d--) {
      $var = substr($h[$d*2], 0, -1);
      if ($a[$r][$d*2] != $mins[$d]) {
        if ($addsemi == 1) { print(";"); } else { $addsemi = 1; }
        print("$var>=$a[$r-$shift][$d*2+1]");
      }
      if ($a[$r][$d*2+1] != $maxs[$d]) {
        if ($addsemi == 1) { print(";"); } else { $addsemi = 1; }
        print("$var<=$a[$r+$shift][$d*2]");
      }
      $addsemi = 1;
      $shift *= $dpts[$d];
    }
    print("]>");
  }
  print("}\n\n");
}

# 12/8/05 - Assign enablings to discrete transitions.
print("#\@.enablings {");
for ($r = 0; $r <= $#a; $r++) {
  $shift = 1;
  for ($d = $dims-1; $d >= 0; $d--) {
    $var = substr($h[$d*2], 0, -1);
    if ($a[$r][$d*2] != $mins[$d]) {
      print("<$h[$d*2]_$r=[$var<=$a[$r-$shift][$d*2+1]]>");
    }
    if ($a[$r][$d*2+1] != $maxs[$d]) {
      print("<$h[$d*2+1]_$r=[$var>=$a[$r+$shift][$d*2]]>");
    }
    $shift *= $dpts[$d];
  }
}
print("}\n\n");

# 12/8/05 - Put rate assignments on the discrete transitions
print("#\@.rate_assignments {");
for ($r = 0; $r <= $#a; $r++) {
  $shift = 1;
  for ($d = $dims-1; $d >= 0; $d--) {
    if ($a[$r][$d*2] != $mins[$d]) {
      print("<$h[$d*2+1]_", ($r-$shift), "=[");
      $addsemi = 0;
      for ($d2 = $dims-1; $d2 >= 0; $d2--) {
        $var = substr($h[$d2*2], 0, -1);
        $rate = $a[$r][$dims*2+$d2];
        if ($addsemi == 1) { print(";"); } else { $addsemi = 1; }
        print("$var:=$rate");
        $addsemi = 1;
      }
      print("]>");
    }
    if ($a[$r][$d*2+1] != $maxs[$d]) {
      print("<$h[$d*2]_", ($r+$shift), "=[");
      $addsemi = 0;
      for ($d2 = $dims-1; $d2 >= 0; $d2--) {
        $var = substr($h[$d2*2], 0, -1);
        $rate = $a[$r][$dims*2+$d2];
        if ($addsemi == 1) { print(";"); } else { $addsemi = 1; }
        print("$var:=$rate");
        $addsemi = 1;
      }
      print("]>");
    }
    $shift *= $dpts[$d];
  }
}
print("}\n\n");

# Specify which things are continuous
print("#\@.continuous ");
# Continuous Variables
for ($d = 0; $d < $dims; $d++) {
  $var = substr($h[$d*2], 0, -1);
  print("$var ");
}
print("\n\n");

# 12/8/05 - Assign initial values to each continuous variable.
print("#\@.init_vals {");
for ($d = 0; $d < $dims; $d++) {
  $var = substr($h[$d*2], 0, -1);
  print("<$var=[0,0]>");
}
print("}\n\n");

# 12/8/05 - Assign initial rates to each continuous variable.
print("#\@.init_rates {");
for ($d = 0; $d < $dims; $d++) {
  $var = substr($h[$d*2], 0, -1);
  $rate = $a[0][$dims*2+$d];
  print("<$var=[$rate,$rate]>");
}
print("}\n\n");

# Initial marking
print(".marking{p_0}\n");
print(".end\n");

print("\n");
