import subprocess
import os
import matplotlib.pyplot as plt 
import numpy as np
import re
import sys

time_serial = []
time_parallel = []
N_list = list(range(2, 24))
TS_list = [4, 16, 64, 256, 1024, 2048, 4096, 8192]

#type the command to run the right executable
exec_name = "./mergesort.run"

# set the number of threads
os.environ["OMP_NUM_THREADS"] = str(16)

## first do the serial part
for N in N_list:
    print(f"Serial N={N}")
    result = subprocess.run([exec_name, str(N)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    res_str = result.stdout.decode('utf-8')
    serial_time = re.findall(r"[-+]?\d*\.\d+|\d+ s", res_str)[1]
    time_serial.append(float(serial_time))

## then do the parallel part
for TS in TS_list:
    _time_parallel = []
    for N in N_list:
        print(f"Parallel N={N}, TS={TS}")
        result = subprocess.run([exec_name, str(N), str(TS)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        res_str = result.stdout.decode('utf-8')
        parallel_time = re.findall(r"[-+]?\d*\.\d+|\d+ s", res_str)[4]
        _time_parallel.append(float(parallel_time))
    time_parallel.append(_time_parallel)


## plot the results
plt.plot(N_list, time_serial, label="Serial")
for i in range(len(TS_list)):
    plt.plot(N_list, time_parallel[i], label=f"Minimum Task Size {TS_list[i]}")
plt.xlabel('N')
plt.ylabel('Average Time (s)')
plt.yscale('log')
plt.legend()
plt.show()
plt.savefig(f"mergesort-TS-perf.png")

