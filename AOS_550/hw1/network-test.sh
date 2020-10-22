#!/bin/bash
network_machine_list_file="./network-test-machine-list.txt"
output_file="network-test-latency.txt"

[ -e $output_file ] && rm $output_file
touch $output_file
while IFS= read -r line
do
	echo "Running for the website >> $line"
	avg=$(ping -c 3 "$line" | tail -1 | awk '{print $4}' | cut -d '/' -f 2)
	echo "$line $avg" >> $output_file
done < "$network_machine_list_file"