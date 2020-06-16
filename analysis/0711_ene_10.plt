unset logscale x
unset logscale y
unset xrange
unset yrange
set xlabel "Energy [MeV]"
set ylabel "Count"
set xrange [0.:9.]
plot "0711_hist02.dat" using 1:8 w boxes title "Without correction", "0711_hist02.dat" using 1:8:9 with yerrorbars notitle