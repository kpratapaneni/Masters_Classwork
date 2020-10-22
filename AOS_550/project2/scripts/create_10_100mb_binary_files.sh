#!/bin/bash

# Generating 100K random files
read -p "Enter Peer Unique Id: " peer_id
for i in `seq 1 10`; do
	dd if=/dev/urandom of=./Files/$peer_id-file-$i-100mb.txt bs=100M count=1
done