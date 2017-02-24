package float;
require Exporter;

our @ISA = qw(Exporter);
our @EXPORT  = qw(compare subtract add int_multiply divide int_divide etof ftoi);
our $VERSION = 1.00;

#This file is a library of subroutines that I wrote to deal with the
#type of numbers that SPICE outputs.  Perl likes to deal with these as
#strings, so I wrote a series of functions to do simple comparisons,
#mathematical computations, and translation to other types.

sub compare{
  #Compares two float values of form ?.*e(+-)??
  #Usage: compare(a,b); returns 1 if a > b, -1 if a < b, 0 if a == b
  my $a = $_[0];
  my $b = $_[1];
  $a =~ m/(.*)e(.*)/; #Saves the numerical values into different variables
  my $f1 = $1; #f is the mantissa
  my $e1 = $2; #e is the exponent
  $b =~ m/(.*)e(.*)/;
  my $f2 = $1;
  my $e2 = $2;
  while (abs($f1) < 1 && $f1 != 0){ #If it is not in scientific format
    $f1 *= 10; #Make it so
    $e1++;
  }
  while (abs($f1) >= 10){
    $f1 /= 10;
    $e1--;
  }
  while (abs($f2) < 1 && $f2 != 0){
    $f2 *= 10;
    $e2++;
  }
  while (abs($f2) >= 10){
    $f2 /= 10;
    $e2--;
  }
  if ($a =~ m/^[-]/ and $b =~ m/^[-]/){ #If they are both negative
    $a =~ s/^[-]//; #Make them both positive
    $b =~ s/^[-]//;
    return compare($a,$b) * -1; #And recursively call the function
  }
  elsif ($a =~ m/^[-]/){ #If only one of them is negative
    return -1; #That's the smaller one
  }
  elsif ($b =~ m/^[-]/){ #Same thing
    return 1;
  }
  elsif ($a =~ m/e[-]/ and $b =~ m/e[-]/){ #If they both have negative exponents
    $a =~ s/e-/e+/; #Make the exponents positive
    $b =~ s/e-/e+/;
    return compare($a,$b) * -1; #And recursively call the function
  }
  elsif ($a =~ m/e[-]/){ #If only one of the exponents is negative
    if ($f2 == 0.000000 || $b == 0){ #And the other number is not zero
      return 1;
    }
    else{
      return -1; #The one with the negative exponent is smaller
    }
  }
  elsif ($b =~ m/e[-]/){ #Same thing
    if ($f1 == 0.000000 || $a == 0){
      return -1;
    }
    else{
      return 1;
    }
  }
  else{ #If they are both positive and have positive exponents
    $a =~ m/(.*)e[+](.*)/;
    my $f1 = $1;
    my $e1 = $2;
    $b =~ m/(.*)e[+](.*)/;
    my $f2 = $1;
    my $e2 = $2;
    if($f1 == 0.000000 and $f2 != 0.00){ #If one is zero
      return -1; #Then it is the smaller one
    }
    elsif($f2 == 0.000000 and $f1 != 0.00){ #Same thing
      return 1;
    }
    elsif ($e1 < $e2){ #The one with the smaller exponent is smaller
      return -1;
    }
    elsif($e1 > $e2){
      return 1;
    } #Or, if the exponents are the same
    elsif ($f1 < $f2){ #Then the one with the smaller mantissa is smaller
      return -1;
    }
    elsif($f1 > $f2){
      return 1;
    }
    else{ #Or they are the same
      return 0;
    }
  }
}

sub subtract{
  #Subtracts two float values of the same form as compare
  #Usage: a - b == subtract(a,b)
  my $a = $_[0];
  my $b = $_[1];
  $a =~ m/(.*)e(.*)/; #Saves the mantissa and exponent, per above
  my $f1 = $1;
  my $e1 = $2;
  $b =~ m/(.*)e(.*)/;
  my $f2 = $1;
  my $e2 = $2;
  my $temp = $e2-$e1; #Finds the difference between the exponents
  if ($f1 == 0.000){ #If the first is zero, then the answer equals
                     #the opposite of the other
    $f2 *= -1;
    my $eout = $e2;
    if ($eout >= 0){
      $eout = sprintf "+%02d", $eout;
    }
    else{
      $eout = sprintf "%03d", $eout;
    }
    $fout = sprintf "%.8fe%s", $f2, $eout; #Formatted properly
    return $fout;
  }
  elsif ($f2 == 0.000){ #If the second is zero, the answer equals the first
    my $eout = $e1;
    if ($eout >= 0){
      $eout = sprintf "+%02d", $eout;
    }
    else{
      $eout = sprintf "%03d", $eout;
    }
    $fout = sprintf "%.8fe%s", $f1, $eout; #Formatted properly
    return $fout;
  }
  if ($temp > 5){ #If they have significantly different orders of magnitude
    my $eout = $e2; #Ignore the smaller one
    if ($eout >= 0){
      $eout = sprintf "+%02d", $eout;
    }
    else{
      $eout = sprintf "%03d", $eout;
    }
    $fout = sprintf "%.8fe%s", $f2, $eout;
    return $fout;
  }
  elsif ($temp < -5){ #Same thing reversed
    my $eout = $e1;
    if ($eout >= 0){
      $eout = sprintf "+%02d", $eout;
    }
    else{
      $eout = sprintf "%03d", $eout;
    }
    $fout = sprintf "%.8fe%s", $f1, $eout;
    return $fout;
  }
  my $fout = $f1-$f2*10**($e2-$e1); #Otherwise, set up the
                                #mathematical equation as makes sense
  my $eout = $e1;
  while (abs($fout) < 1 && $fout != 0){ #Get the answer in scientific notation
    $fout *= 10;
    $eout--;
  }
  while ((abs($fout)+0.000001) >= 10){
    $fout /= 10;
    $eout++;
  }
  if($eout >= 0){
    $eout = sprintf "+%02d", $eout; #Format the answer
  }
  else{
    $eout = sprintf "%03d", $eout;
  }
  my $out = sprintf "%.8fe%s", $fout, $eout;
  return $out; #And return it
}

sub add{
  #Adds two float values as same form as compare
  #Usage: a+b == add(a,b)
  my $a = $_[0]; #Does the same thing as the early part of subtract
  my $b = $_[1];
  $a =~ m/(.*)e(.*)/;
  my $f1 = $1;
  my $e1 = $2;
  $b =~ m/(.*)e(.*)/;
  my $f2 = $1;
  my $e2 = $2;
  my $temp = ($e2-$e1);
  if ($temp > 5){ #If a big difference between magnitude of numbers
    return $f2; #Ignore the smaller one
  }
  elsif ($temp < -5){ #Same thing
    return $f1;
  }
  my $fout = $f1+$f2*10**($e2-$e1); #Else, add as makes sense
  $eout = $e1;
  while (abs($fout) < 1 && $fout != 0){
    $fout *= 10; #Get the answer in scientific notation
    $eout--;
  }
  while (abs($fout) >= 10){
    $fout /= 10;
    $eout++;
  }
  if($eout>=0){
    $eout = sprintf "+%02d", $eout; #format it
  }
  else{
    $eout = sprintf "%03d", $eout;
  }
  my $out = sprintf "%.8fe%s", $fout, $eout;
  return $out; #And return it
}

sub int_multiply{
  #Multiplies an integer by a float
  #Usage: float*int == multiply(float,int)
  my $a = $_[0]; #Same as previous functions (see subtract)
  my $b = $_[1];
  $a =~ m/(.*)e(.*)/;
  my $f1 = $1;
  my $e1 = $2;
  my $fout = $f1 * $b; #Multiply the mantissa by the integer
  my $eout = $e1;
  if ($f1 == 0.000){ #If it equals zero
    return sprintf "0.00000000e+00"; #Return a formatted zero
  }
  while (abs($fout) < 1){ #Get in scientific notation
    $fout *= 10;
    $eout--;
  }
  while (abs($fout) >= 10){
    $fout /= 10;
    $eout++;
  }
  if($eout>=0){
    $eout = sprintf "+%02d", $eout; #Format
  }
  else{
    $eout = sprintf "%03d", $eout;
  }
  my $out = sprintf "%.8fe%s", $fout, $eout;
  return $out; #And return
}

sub divide{
  #Divides two float values of same form as compare
  #Usage: a/b == divide(a,b)
  my $a = $_[0]; #Same as above (see subtract)
  my $b = $_[1];
  $a =~ m/(.*)e(.*)/;
  my $f1 = $1;
  my $e1 = $2;
  $b =~ m/(.*)e(.*)/;
  my $f2 = $1;
  my $e2 = $2;
  if ($f1 == 0.000){ #If dividend is zero
    return sprintf "0.00000000e+00"; #Return formatted zero
  } #Else
  $fout = $f1/$f2; #Divide mantissas
  $eout = $e1-$e2; #Subtract exponents
  while (abs($fout) < 1){ #Get in scientific notation
    $fout *= 10;
    $eout--;
  }
  while (abs($fout) >= 10){
    $fout /= 10;
    $eout++;
  }
  if($eout>=0){
    $eout = sprintf "+%02d", $eout; #Format
  }
  else{
    $eout = sprintf "%03d", $eout;
  }
  my $out = sprintf "%.8fe%s", $fout, $eout;
  return $out; #And return
}

sub int_divide{
  #Divides an integer value from a float (with same form as compare)
  #Usage: float/int == int_divide(float/int)
  my $a = $_[0]; #Same as above (see subtract)
  my $b = $_[1];
  $a =~ m/(.*)e(.*)/;
  my $f1 = $1;
  my $e1 = $2;
  my $fout = $f1/$b; #Divide mantissa by integer
  my $eout = $e1; #Copy exponent to output
  while (abs($fout) < 1 && $fout != 0){ #Get in scientific notation
    $fout *= 10;
    $eout--;
  }
  while (abs($fout) >= 10){
    $fout /= 10;
    $eout++;
  }
  if($eout>=0){
    $eout = sprintf "+%02d", $eout; #Format
  }
  else{
    $eout = sprintf "%03d", $eout;
  }
  my $out = sprintf "%.8fe%s", $fout, $eout;
  return $out; #And return
}

sub etof{
  #Changes a float (of same form as compare) to a decimal, sans exp
  my $a = $_[0];
  $a =~ m/(.*)e(.*)/;
  my $f = $1;
  my $e = $2;
  $f *= (10**$e); #The mantissa equals itself times ten to the power
                  #of the exponent
  return $f; #Leave it as a floating point decimal
}

sub ftoi{
  #Changes a float (of same form as compare) to an integer
  my $a = $_[0];
  $a =~ m/(.*)e(.*)/;
  my $f = $1;
  my $e = $2;
  $f *= (10**$e); #The mantissa equals itself times ten to the power
                  #of the exponent
  return int $f; #Change the answer to an integer
}
