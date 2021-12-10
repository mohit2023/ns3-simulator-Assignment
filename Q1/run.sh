# ./run.sh protocolName
cd ../..
./waf --run "scratch/first --protocol=$1"
cp first.cwnd 2019CS10372/Q1
cd 2019CS10372/Q1
gnuplot -e "filename='$1'" plot.plt
rm first.cwnd