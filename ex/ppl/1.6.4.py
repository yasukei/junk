#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""0からn-1までの間のk個の重複のない整数列を生成する"""

import random

def make_rseq(n, k):
	seq = []

	for i in range(k):
		rand = random.randint(0, n - 1)
		while rand in seq:
			rand = random.randint(0, n - 1)
		seq.append(rand)

	return seq

if __name__ == '__main__':
	n = 10
	k = n/2
	for i in make_rseq(n, k):
		print i

