import sys
import argparse

parser = argparse.ArgumentParser(description='a clone of cat')

parser.add_argument('files', metavar='FILES',
                    nargs='*',
                    type=argparse.FileType('r'),
                    default=[sys.stdin],
                    help='file paths')
parser.add_argument('-n', action='store_true',
                    help='output with line numbers')

class Printer():
    def __call__(self, line):
        print(line, end='')

class PrinterWithLineNo():
    def __init__(self, line_no=1):
        self.line_no = line_no
    def __call__(self, line):
        line = '{0}\t{1}'.format(self.line_no, line)
        self.line_no += 1
        print(line, end='')

def print_files(files, printer):
    for file in files:
        for line in file:
            printer(line)
        print('')

def cat(files, n=False):
    if n:
        printer = PrinterWithLineNo()
    else:
        printer = Printer()

    print_files(files, printer)

def main(argv):
    args = vars(parser.parse_args(argv[1:]))
    cat(args['files'], args['n'])
    
if __name__ == "__main__":
    main(sys.argv)
