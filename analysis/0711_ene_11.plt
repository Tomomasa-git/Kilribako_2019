unset logscale x
unset logscale y
unset xrange
unset yrange
set xlabel "Energy [MeV]"
set ylabel "Count"
set xrange [0.:7.]
set yrange [0.:13.]
plot "0711_hist04.dat" using 1:4 w boxes title " Energy ", "0711_hist04.dat" using 1:4:5 with yerrorbars notitle