#!/bin/bash

echo "------------">>$1
date>>$1
iwconfig wlp3s0 | grep -e "ESSID\|Access Point">>$1
ifconfig wlp3s0 | grep -w "inet\|ether">>$1
