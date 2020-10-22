#!/bin/bash
#Below command will redirect the output and error logs into the given file
exec &> disk-benchmark-background-log.txt
time dd if=/dev/zero of=/tmp/benchmark bs=4000k count=10000; sync