#!/bin/bash

# Generating 100K random files
read -p "Enter Peer Unique Id: " peer_id
for i in `seq 1 100000`; do
	dd if=/dev/urandom of=./Files/$peer_id-file-$i-10kb.txt bs=10k count=1
done