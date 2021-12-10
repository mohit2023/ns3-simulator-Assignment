set terminal png
set output 'parta_' . filename . '_plot.png'
set title "Congestion Window Size VS Time"
set xlabel "Time(seconds)"
set ylabel "Congestion Window Size"
plot "second_1.cwnd" using 1:2 ps 0.5 pt 7 title filename . "Mbps Channel data rate"