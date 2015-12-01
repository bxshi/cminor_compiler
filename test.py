#!/usr/bin/python

from os import listdir
from os.path import isfile, join
import subprocess

# Compile library

subprocess.call(["gcc", "-S", "./src/codegen/library.c", "-o", "library.s"])

TEST_PATH = "./test/"

test_files = [f.split(".")[0] for f in listdir(TEST_PATH) if isfile(join(TEST_PATH,f))]

test_files = [f for f in test_files if f != ""]

for fname in test_files:
	print " ".join(["test",fname])
	cminor_path = "".join([TEST_PATH, fname, ".cminor"])
	s_path = "".join([fname, ".s"])

	subprocess.call(["./cminor", "-codegen", cminor_path, s_path])

	bin_path = "".join([fname, ".out"])
	subprocess.call(["gcc", "-s", s_path, "library.s", "-o", bin_path])

	rtn = subprocess.call(["".join(["./",bin_path])])
	if rtn == 0:
		print " ".join(["\t\t\t",bin_path, "okay"])
	else:
		print " ".join(["\t\t\t",bin_path, "returns",str(rtn)])


