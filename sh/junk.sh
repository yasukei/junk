#!/bin/sh

isContainedInArray ()
{
	target=$1
	shift
	array=$*

	for elem in ${array}
	do
		if [ ${target} = ${elem} ]; then
			true; return
		fi
	done

	false; return
}

