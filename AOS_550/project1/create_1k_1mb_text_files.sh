#!/bin/bash

read -p "Enter Peer Unique Id: " peer_id

for i in `seq 1 1000`; do
	dd if=/dev/urandom of=/srv/shared/$peer_id-file-$i-1mb.txt bs=1M count=1
done