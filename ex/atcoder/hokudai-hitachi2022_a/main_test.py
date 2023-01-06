import unittest

from main import *

# -----------------------------------------------------------------------------
# RewardFunctionTest
# -----------------------------------------------------------------------------
class RewardFunctionTest(unittest.TestCase):
    def test1(self):
        rewards = [2, 2]
        reward_func = RewardFunction(rewards)
        self.assertEqual(reward_func(0), 2)
        self.assertEqual(reward_func(1), 2)
        self.assertEqual(reward_func(2), 2)
        self.assertEqual(reward_func(3), 2)
        self.assertEqual(reward_func(4), 2)

    def test2(self):
        rewards = [2, 2, 4, 4]
        reward_func = RewardFunction(rewards)
        self.assertEqual(reward_func(0), 2)
        self.assertEqual(reward_func(1), 2)
        self.assertEqual(reward_func(2), 2)
        self.assertEqual(reward_func(3), 3)
        self.assertEqual(reward_func(4), 4)
        self.assertEqual(reward_func(5), 4)

    def test3(self):
        rewards = [2, 2, 4, 3]
        reward_func = RewardFunction(rewards)
        self.assertEqual(reward_func(0), 2)
        self.assertEqual(reward_func(1), 2)
        self.assertEqual(reward_func(2), 2)
        self.assertEqual(reward_func(3), 2.5)
        self.assertEqual(reward_func(4), 3)
        self.assertEqual(reward_func(5), 3)

    def test4(self):
        rewards = [2, 0, 4, 10, 6, 10, 8, 0]
        reward_func = RewardFunction(rewards)
        self.assertEqual(reward_func(0), 0)
        self.assertEqual(reward_func(1), 0)
        self.assertEqual(reward_func(2), 0)
        self.assertEqual(reward_func(3), 5)
        self.assertEqual(reward_func(4), 10)
        self.assertEqual(reward_func(5), 10)
        self.assertEqual(reward_func(6), 10)
        self.assertEqual(reward_func(7), 5)
        self.assertEqual(reward_func(8), 0)
        self.assertEqual(reward_func(9), 0)

