#!/usr/bin/bash

make clean &> /dev/null
sudo rmmod no &> /dev/null

make
sudo insmod no.ko
sudo chmod 666 /dev/no
