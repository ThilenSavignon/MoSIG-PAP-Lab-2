import subprocess
import os
import matplotlib.pyplot as plt 
import numpy as np
import re
import sys

if len(sys.argv) != 3:
    print(sys.argv[0], "<exec_name>", "max_N")
    exit(1)

nr_threads_list = [2, 4, 8, 16]
time_serial = []
time_parallel = []
N_list = list(range(2, int(sys.argv[2]) + 1))

#type the command to run the right executable
exec_name = sys.argv[1]

## first do the serial part
for N in N_list:
    print(f"Serial N={N}")
    os.environ["OMP_NUM_THREADS"] = str(nr_threads_list[-1])
    result = subprocess.run([exec_name, str(N)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    res_str = result.stdout.decode('utf-8')
    serial_time = re.findall(r"[-+]?\d*\.\d+|\d+ s", res_str)[1]
    time_serial.append(float(serial_time))

## then do the parallel part
for nr_threads in nr_threads_list:
    # set the number of threads
    os.environ["OMP_NUM_THREADS"] = str(nr_threads)
    _time_parallel = []
    for N in N_list:
        print(f"Parallel N={N}, Threads={nr_threads}")
        result = subprocess.run([exec_name, str(N), str(1)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        res_str = result.stdout.decode('utf-8')
        parallel_time = re.findall(r"[-+]?\d*\.\d+|\d+ s", res_str)[4]
        _time_parallel.append(float(parallel_time))
    time_parallel.append(_time_parallel)


## plot the results
plt.plot(N_list, time_serial, label="Serial")
for i in range(len(nr_threads_list)):
    plt.plot(N_list, time_parallel[i], label=f"Parallel {nr_threads_list[i]} threads")
plt.xlabel('N')
plt.ylabel('Average Time (s)')
plt.yscale('log')
plt.legend()
plt.show()
plt.savefig(f"{exec_name[2:-4]}-perf.png")

