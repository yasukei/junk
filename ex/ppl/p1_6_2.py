#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""and, or, shift などのビットレベルの論理演算子を使って、ビット列を実装する"""

import sys
import math
import unittest

class Nbits:
	"""任意のビット列を表現する。かなり遅い。python組み込みのリストの方が速い"""

	def __init__(self, nbits=32):
		self.nbits = nbits
		bitof_elem = int(math.log(sys.maxint + 1, 2)) + 1
		self.bit_mask_digit = int(math.log(bitof_elem, 2))
		numof_elems = (nbits >> self.bit_mask_digit) + 1
		self.elems = [0] * numof_elems

		self.bit_mask = 0
		for i in range(self.bit_mask_digit):
			self.bit_mask = self.bit_mask | (1 << i)

	def __get_elem_index(self, nbit):
		return nbit >> self.bit_mask_digit

	def __get_index_in_elem(self, nbit):
		return nbit & self.bit_mask

	def __get_elem_bit(self, nbit):
		if (nbit <= -1) or (self.nbits <= nbit):
			raise IndexError
		return self.__get_elem_index(nbit), self.__get_index_in_elem(nbit)

	def n(self):
		return self.nbits

	def set(self, nbit):
		elem_index, index_in_elem = self.__get_elem_bit(nbit)
		self.elems[elem_index] = self.elems[elem_index] | (1 << index_in_elem)

	def reset(self, nbit):
		elem_index, index_in_elem = self.__get_elem_bit(nbit)
		self.elems[elem_index] = self.elems[elem_index] & (0 << index_in_elem)

	def test(self, nbit):
		elem_index, index_in_elem = self.__get_elem_bit(nbit)
		return (self.elems[elem_index] >> index_in_elem) & 1

class TestNbits(unittest.TestCase):
	n = 0
	def setUp(self):
		self.__Nbits = Nbits(self.n)

	def tearDown(self):
		del self.__Nbits

	def test_n(self):
		self.assertEqual(self.n, self.__Nbits.n())

	def test_set_IndexError(self):
		self.assertRaises(IndexError, self.__Nbits.set, -sys.maxint-1)
		self.assertRaises(IndexError, self.__Nbits.set, -1)
		self.assertRaises(IndexError, self.__Nbits.set, self.n)
		self.assertRaises(IndexError, self.__Nbits.set, sys.maxint)

	def test_reset_IndexError(self):
		self.assertRaises(IndexError, self.__Nbits.reset, -sys.maxint-1)
		self.assertRaises(IndexError, self.__Nbits.reset, -1)
		self.assertRaises(IndexError, self.__Nbits.reset, self.n)
		self.assertRaises(IndexError, self.__Nbits.reset, sys.maxint)

	def test_test_IndexError(self):
		self.assertRaises(IndexError, self.__Nbits.test, -sys.maxint-1)
		self.assertRaises(IndexError, self.__Nbits.test, -1)
		self.assertRaises(IndexError, self.__Nbits.test, self.n)
		self.assertRaises(IndexError, self.__Nbits.test, sys.maxint)

	def test_set(self):
		for i in range(self.n):
			self.assertEqual(0, self.__Nbits.test(i))

			self.__Nbits.set(i)

			self.assertEqual(1, self.__Nbits.test(i))

	def test_reset(self):
		for i in range(self.n):
			self.__Nbits.set(i)
			self.assertEqual(1, self.__Nbits.test(i))

			self.__Nbits.reset(i)

			self.assertEqual(0, self.__Nbits.test(i))

	def test_test(self):
		for i in range(self.n):
			self.assertEqual(0, self.__Nbits.test(i))

class Test1bits(TestNbits):
	n = 1
class Test2bits(TestNbits):
	n = 2
class Test7bits(TestNbits):
	n = 7
class Test8bits(TestNbits):
	n = 8
class Test9bits(TestNbits):
	n = 9
class Test31bits(TestNbits):
	n = 31
class Test32bits(TestNbits):
	n = 32
class Test33bits(TestNbits):
	n = 33
class Test63bits(TestNbits):
	n = 63
class Test64bits(TestNbits):
	n = 64
class Test65bits(TestNbits):
	n = 65
class Test127bits(TestNbits):
	n = 127
class Test128bits(TestNbits):
	n = 128
class Test129bits(TestNbits):
	n = 129

if __name__ == '__main__':
	unittest.main()
