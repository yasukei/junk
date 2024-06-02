#!/bin/bash

network_addr=100.64.1.
indices=`seq 2 254`
valid_addrs=()

for i in ${indices}; do
	echo ${i}
	ping -q -c 1 -W 1 "${network_addr}${i}"
	if [ $? -eq 0 ] ; then
		valid_addrs+="${network_addr}${i}"
	fi
done

echo "The following addresses succeeded to ping"
for addr in ${valid_addrs}; do
	echo ${addr}
done
