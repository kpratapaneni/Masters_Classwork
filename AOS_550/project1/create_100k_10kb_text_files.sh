#!/bin/bash

read -p "Enter Peer Unique Id: " peer_id
for i in `seq 1 10000`; do
	dd if=/dev/urandom of=/Users/kalpana/eclipse-workspace/Peer-4/Files/$peer_id-file-$i-10kb.txt bs=10k count=1
done