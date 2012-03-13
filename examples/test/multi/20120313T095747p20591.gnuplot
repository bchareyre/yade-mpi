#!/usr/bin/env gnuplot
#
# created Tue Mar 13 09:57:48 2012 (20120313_09:57)
#
set term wxt 0 persist
set xlabel 'y_sph'
set grid
set datafile missing 'nan'
set ylabel 'z_sph'
plot  "< bzcat 20120313T095747p20591.data.bz2" using 2:3 title '← z_sph(y_sph)' with lines
