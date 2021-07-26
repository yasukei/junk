import mycat

import unittest
from unittest.mock import patch
from io import StringIO


class TestMyCat(unittest.TestCase):
            
    @patch('sys.stdout', new_callable=StringIO)
    def test_cat_two_files(self, mock_stdout):
        files = [StringIO('a'), StringIO('b')]
        mycat.cat(files, False)
        self.assertEqual(mock_stdout.getvalue(), 'a\nb\n')
        

if __name__ == "__main__":
    unittest.main()
