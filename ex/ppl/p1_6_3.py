#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""0からn-1までの間のk個の重複のない整数列をソートする"""

import sys
import getopt

def sort_uniqseq1(n, k, uniqseq=[]):
	"""ソートアルゴリズム1"""
	bucket = [False] * n
	sorted_seq = []

	for i in uniqseq:
		bucket[i] = True
	for i in range(len(bucket)):
		if bucket[i]:
			sorted_seq.append(i)

	return sorted_seq

class Config:
	def __init__(self, n=10000000, k=1000000, filename="", m=sort_uniqseq1, q=False):
		self.n = n
		self.k = k
		self.filename = "p1-6-3input.txt"
		self.m = m
		self.q = q

def parse_args():
	config = Config()

	try:
		opts, args = getopt.getopt(sys.argv[1:], "n:k:m:q")
	except getopt.GetoptError, err:
		print str(err)
		sys.exit(1)

	for o, a in opts:
		if o == "-n":
			config.n = int(a)
		elif o == "-k":
			config.k = int(a)
		elif o == "-m":
			if a == "1":
				config.m = sort_uniqseq1
			else:
				assert False, "invalid option"
		elif o == "-q":
			config.q = True
		else:
			assert False, "unhandled option"
	#config.filename = args[1]

	return config

def main():
	config = parse_args()

	seq = []
	with open(config.filename) as f:
		for line in f:
			seq.append(int(line))

	for i in config.m(config.n, config.k, seq):
		if not config.q:
			print i

if __name__ == '__main__':
	main()

