#!/bin/bash

case $1 in

    "on")
      sudo airmon-ng start  wlp3s0
      ;;
    "off")
      sudo airmon-ng stop  wlp3s0mon
      ;;

     "check")
     sudo airmon-ng check kill

      ;;

     "reset")
     service network-manager start

      ;;
      *)
      echo 'Options on, off, check, reset'
      ;;

  esac
