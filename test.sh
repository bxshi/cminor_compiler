#!/bin/sh

for testfile in ./test/*.cminor
do
	if cminor -parse $testfile > $testfile.out
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done
