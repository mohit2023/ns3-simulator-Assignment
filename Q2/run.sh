# ./run.sh <part> <dataRate>
cd ../..
if [ "$1" == "1" ]; then
  ./waf --run "scratch/second_1 --rate=$2"
  cp second_1.cwnd 2019CS10372/Q2
  cd 2019CS10372/Q2
  gnuplot -e "filename='$2'" plot1.plt
  rm second_1.cwnd
elif [ "$1" == "2" ]; then
  ./waf --run "scratch/second_2 --rate=$2"
  cp second_2.cwnd 2019CS10372/Q2
  cd 2019CS10372/Q2
  gnuplot -e "filename='$2'" plot2.plt
  rm second_2.cwnd
else
  echo "Error"
fi
