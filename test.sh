#!/bin/sh

for testfile in ./test/good*.cminor
do
	if cminor -codegen $testfile $testfile.s
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done
