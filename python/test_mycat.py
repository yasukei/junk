import mycat

import unittest
from unittest.mock import patch
from io import StringIO

class TestMyCat:
    @patch('sys.stdout', new_callable=StringIO)
    def test_mycat(self, mock_stdout):
        mycat.cat(self.args['files'], self.args['n'])
        self.assertEqual(mock_stdout.getvalue(), self.args['expect'])


class TestMyCat1(TestMyCat, unittest.TestCase):
    args = {
        'files' : [StringIO('')],
        'n' : False,
        'expect' : '\n',
        }        

class TestMyCat2(TestMyCat, unittest.TestCase):
    args = {
        'files' : [StringIO('a'), StringIO('b')],
        'n' : False,
        'expect' : 'a\nb\n',
        }        

if __name__ == "__main__":
    unittest.main()
