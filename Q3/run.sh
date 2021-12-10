# ./run.sh <conf>
cd ../..
./waf --run "scratch/third --conf=$1"
cp third_1.cwnd 2019CS10372/Q3
cp third_2.cwnd 2019CS10372/Q3
cp third_3.cwnd 2019CS10372/Q3
cd 2019CS10372/Q3
python3 plot.py $1 "1"
python3 plot.py $1 "2"
python3 plot.py $1 "3"
rm third_1.cwnd
rm third_2.cwnd
rm third_3.cwnd