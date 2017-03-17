import os
import sys
import subprocess

KB = 1024
MB = 1024 * 1024
sizes = [
    1,
    1/2,
    1/4,
    1/8,
    1/16,
    1/32,
    1/64,
    1/128,
    1/256,
    1/65536
]

mem_size = 200*MB


filename = "records.dat"

block_size = 2*MB


f1 = open("performance_disk_sort.txt", "w")

for size in sizes:

    filename = "records"+str(count)+".dat"
    subprocess.run(["/usr/bin/time", "-v", "disk_sort", filename, str(mem_size*size), str(block_size)], stdout=f1)
    count = count + 1;

    subprocess.run(["/usr/bin/time", "-v", "disk_sort", filename, str(mem_size*size), str(block_size)], stdout=f1)

    print("\n\n")






