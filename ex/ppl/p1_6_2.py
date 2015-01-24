#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""and, or, shift などのビットレベルの論理演算子を使って、ビット列を実装する"""

import sys
import math
import unittest

class Nbits:
	def __init__(self, nbits=32):
		self.nbits = nbits
		self.bitof_elem = int(math.log(sys.maxint + 1, 2)) + 1
		numof_elems = (nbits >> int(math.log(self.bitof_elem, 2))) + 1
		self.elems = [0] * numof_elems

	def __get_elem_index(self, nbit):
		return nbit >> int(math.log(self.bitof_elem, 2))

	def __get_index_in_elem(self, nbit):
		return (nbit >> self.bitof_elem) << self.bitof_elem

	def __get_elem_bit(self, nbit):
		if nbit >= self.nbits:
			raise IndexError
		return self.__get_elem_index(nbit), self.__get_index_in_elem(nbit)

	def n(self):
		return self.nbits

	def set(self, nbit):
		elem_index, index_in_elem = self.__get_elem_bit(nbit)
		self.elems[elem_index] = self.elems[elem_index] | (1 << index_in_elem)

	def reset(self, nbit):
		if nbit >= self.nbits:
			raise IndexError
		elem_index = self.__get_elem_index(nbit)
		index_in_elem = self.__get_index_in_elem(nbit)
		self.elems[elem_index] = self.elems[elem_index] & (0 << index_in_elem)

	def test(self, nbit):
		if nbit >= self.nbits:
			raise IndexError
		elem_index = self.__get_elem_index(nbit)
		index_in_elem = self.__get_index_in_elem(nbit)
		return self.elems[elem_index] & (1 << index_in_elem)

class Test0bits(unittest.TestCase):
	def setUp(self):
		self.__0bits = Nbits(0)

	def tearDown(self):
		del self.__0bits

	def test_n(self):
		self.assertEqual(0, self.__0bits.n())

	def test_set_IndexError(self):
		self.assertRaises(IndexError, self.__0bits.set, 0)

	def test_reset_IndexError(self):
		self.assertRaises(IndexError, self.__0bits.reset, 0)

	def test_test_IndexError(self):
		self.assertRaises(IndexError, self.__0bits.test, 0)

class Test1bits(unittest.TestCase):
	def setUp(self):
		self.__1bits = Nbits(1)

	def tearDown(self):
		del self.__1bits

	def test_n(self):
		self.assertEqual(1, self.__1bits.n())

	def test_set_IndexError(self):
		self.assertRaises(IndexError, self.__1bits.set, 1)

	def test_reset_IndexError(self):
		self.assertRaises(IndexError, self.__1bits.reset, 1)

	def test_test_IndexError(self):
		self.assertRaises(IndexError, self.__1bits.test, 1)

	def test_set(self):
		self.assertEqual(0, self.__1bits.test(0))

		self.__1bits.set(0)

		self.assertEqual(1, self.__1bits.test(0))

	def test_reset(self):
		self.__1bits.set(0)
		self.assertEqual(1, self.__1bits.test(0))

		self.__1bits.reset(0)

		self.assertEqual(0, self.__1bits.test(0))

	def test_test(self):
		self.assertEqual(0, self.__1bits.test(0))

if __name__ == '__main__':
	unittest.main()
