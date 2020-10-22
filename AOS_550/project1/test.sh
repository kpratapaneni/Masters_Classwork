#!/bin/bash

array1=()
for f in /Users/kalpana/eclipse-workspace/Peer-1/Files/*.txt
do
  array1+=(${f##*/})
done

array2=()
for f in /Users/kalpana/eclipse-workspace/Peer-2/Files/*.txt
do
  array2+=(${f##*/})
done

array3=()
for f in /Users/kalpana/eclipse-workspace/Peer-3/Files/*.txt
do
  array3+=(${f##*/})
done

INDEX=0
for i in "${!array1[@]}"; do 
  cat "${array1[$INDEX]}" >> input.txt
  cat "${array2[$INDEX]}" >> input.txt
  echo "${array3[$INDEX]}" >> input.txt
  INDEX=$((INDEX+1))
done