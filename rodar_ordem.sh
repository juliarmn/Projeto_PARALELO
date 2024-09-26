#!/bin/bash

gcc manager.c -o m
gcc worker.c -o w

./m &

sleep 1

for i in {0..7}; do
    gnome-terminal -- bash -c "./w $i; echo 'Pressione Enter para fechar...'; read"
    sleep 1
done

rm w
rm m
