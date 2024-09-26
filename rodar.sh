#!/bin/bash

gcc manager.c -o m
gcc worker.c -o w

./m &

sleep 1

./w 0 &
sleep 1
./w 2 &
sleep 1
./w 4 &
sleep 1
./w 3 &
sleep 1
./w 6 &
sleep 1
./w 7 &
sleep 1
./w 5 &
sleep 1
./w 1

wait

rm w
rm m
