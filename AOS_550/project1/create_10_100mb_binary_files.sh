#!/bin/bash

read -p "Enter Peer Unique Id: " peer_id
for i in `seq 1 10`; do
	dd if=/dev/urandom of=/srv/shared/$peer_id-file-$i-100mb.bin bs=100M count=1
done