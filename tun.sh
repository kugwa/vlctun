#!/bin/bash

if [ "$1" == "" ]; then
	ip tuntap
elif [ "$1" == "add" ]; then
	ip tuntap add "$2" mode tun user "$3"
	ip link set "$2" up
	ip addr add "$4" dev "$2"
elif [ "$1" == "del" ]; then
	ip link set "$2" down
	ip tuntap del "$2" mode tun
else
	echo "Usage: tun.sh"
	echo "       tun.sh add PHYS_DEV USER IFADDR"
	echo "       tun.sh del PHYS_DEV"
fi
