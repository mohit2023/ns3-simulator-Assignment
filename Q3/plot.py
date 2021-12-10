import sys
import matplotlib.pyplot as plt

arg = sys.argv[1]
connection = sys.argv[2]
filename = "third_" + connection + ".cwnd"

time = []
cwnd = []

f = open(filename, "r")
for line in f:
  arr = line.split()
  time.append(float(arr[0]))
  cwnd.append(float(arr[1]))

plt.plot(time, cwnd, label = "Configuration "+arg + " : Connection "+connection)
plt.xlabel('Time (sec)')
plt.ylabel('Congestion Window')
plt.title('Congestion Window vs Time')
plt.legend()
plt.show()
plt.grid()
output = "conf"+arg+"_connection"+connection+"_plot.png"
plt.savefig(output)