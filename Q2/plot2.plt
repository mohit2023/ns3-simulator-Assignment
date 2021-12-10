set terminal png
set output 'partb_' . filename . '_plot.png'
set title "Congestion Window Size VS Time"
set xlabel "Time(seconds)"
set ylabel "Congestion Window Size"
plot "second_2.cwnd" using 1:2 ps 0.5 pt 7 title filename . "Mbps Application data rate"