import subprocess
import csv

table = [[0 for i in range(256)] for j in range(20)]
for j in range(20):
	for i in range(256):
		table[0][i] = i + 1
		command = "bash test.sh " + str(i + 1)
		out = subprocess.check_output(command, shell=True)
		cutted = out.decode()[2:-1]
		decoded = int(cutted)
		table[j][i] = cutted

with open("measure.csv", mode='w') as csv_file:
	csv_writer = csv.writer(csv_file, delimiter=';', quoting=csv.QUOTE_MINIMAL)
	for row in table:
		csv_writer.writerow(row)