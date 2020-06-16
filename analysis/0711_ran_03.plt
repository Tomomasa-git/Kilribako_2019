unset logscale x
unset logscale y
unset xrange
unset yrange
set xlabel "Range [cm]"
set ylabel "Count"
set xrange [0.:8.]
plot "0711_hist03.dat" using 1:2 w boxes title "Without correction", "0711_hist03.dat" using 1:2:3 with yerrorbars notitle, "0711_hist03.dat" using 1:6 w boxes title "With correction", "0711_hist03.dat" using 1:6:7 with yerrorbars notitle