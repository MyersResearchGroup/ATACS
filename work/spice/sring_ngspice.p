plot "sring_ngspice.dat" using 1:2 with linespoints, "sring_ngspice.dat" using 1:3 with linespoints, "sring_ngspice.dat" using 1:4 with linespoints, "sring_ngspice.dat" using 1:5 with linespoints, "sring_ngspice.dat" using 1:6 with linespoints
set out 'osc.ps'
set terminal postscript landscape enhanced "Helvetica" 14 
replot
set size 1,1