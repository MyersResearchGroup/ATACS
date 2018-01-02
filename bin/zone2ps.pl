#!/usr/bin/perl -p
@a = <STDIN>;
$filename = "tmp.ps";
$pstemplate = "zone.ps";
open(pstemp,"$pstemplate");
open(psfile,">$filename");
$b=1;
for ($i = 0; $i <= $#a; $i++)
{
    chomp($a[$i]);
    ($c, $x, $y, $z) = ($a[$i] =~ /^(\S+)\s+(\S+)\s+(\S+)\s*(.*)/);
    if ($i==0) {
	$c0 = $c;
	$c0 =~ s/://g;
	$xmin = $y;
	$ymin = $z;
    } elsif ($i==1) {
	$c1 = $c;
	$c1 =~ s/://g;
	$xmax = $x;
	$yint = $z;
    } else {
	$c2 = $c;
	$c2 =~ s/://g;
	$ymax = $x;
	$xint = $y;
    }
}
$xm = $xmax;
if ($xmax <= 0) {
    $xm = $xmin;
}
$ym = $ymax;
if ($ymax <= 0) {
    $ym = $ymin;
}
if ($xm > $ym) {
    $scale = 50/$xm;
} else {
    $scale = 50/$ym;
}
while (<pstemp>)
{
    chomp;
    s/1 1 scale/$scale $scale scale/g; 
    if (($xmax <= 0) && ($ymax <= 0)) {
	s/100 100 translate/500 500 translate/g;
    } elsif ($xmax <= 0) {
	s/100 100 translate/500 100 translate/g;
    } elsif ($ymax <= 0) {
	s/100 100 translate/100 500 translate/g;
    }
    if (/% Matrix goes here/) {
	$b=0;
    }
    if ($b==1) {
	print(psfile $_, "\n");
    }
}
close(pstemp);
print(psfile "0 ",$xmin," ",$ymin, "\n");
print(psfile $xmax," 0 ",$yint, "\n");
print(psfile $ymax," ",$xint," 0\n");
print(psfile "domatrix\n");
print(psfile "(",$c1,") labelx\n");
print(psfile "(",$c2,") labely\n");
print(psfile "showpage\n");
close(psfile);
#system "ps2epsi tmp.ps";
system "open tmp.ps &";
