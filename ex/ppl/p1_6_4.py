#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""0からn-1までの間のk個の重複のない整数の乱数列を生成する"""

import sys
import getopt
import random

def make_randseq1(n, k):
	"""乱数列を生成　バージョン1"""
	randseq = []

	for i in range(k):
		rand = random.randint(0, n - 1)
		while rand in randseq:
			rand = random.randint(0, n - 1)
		randseq.append(rand)

	return randseq

def make_randseq2(n, k):
	"""乱数列を生成　バージョン2"""
	randseq = []
	bucket = [0] * n

	for i in range(k):
		rand = random.randint(0, n - 1)
		while bucket[rand] == 1:
			rand = random.randint(0, n - 1)
		bucket[rand] = 1
		randseq.append(rand)

	return randseq

def make_randseq3(n, k):
	"""乱数列を生成　バージョン3"""
	randseq = []
	seq = [0] * n

	for i in range(n):
		seq[i] = i
	for i in range(k):
		rand = random.randint(0, len(seq) - 1)
		randseq.append(seq.pop(rand))

	return randseq

def make_randseq4(n, k):
	"""乱数列を生成　バージョン4"""
	seq = [0] * n

	for i in range(n):
		seq[i] = i
	for i in range(k):
		seq[random.randint(i, len(seq) - 1)], seq[i] = seq[i], seq[random.randint(i, len(seq) - 1)]
	for i in range(k, n):
		seq.pop()

	return seq

class Config:
	def __init__(self, n=10000000, k=1000000, m=make_randseq2, q=False):
		self.n = n
		self.k = k
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
				config.m = make_randseq1
			elif a == "2":
				config.m = make_randseq2
			elif a == "3":
				config.m = make_randseq3
			elif a == "4":
				config.m = make_randseq4
			else:
				assert False, "invalid option"
		elif o == "-q":
			config.q = True
		else:
			assert False, "unhandled option"

	return config

def main():
	config = parse_args()

	for i in config.m(config.n, config.k):
		if not config.q:
			print i

if __name__ == '__main__':
	main()


