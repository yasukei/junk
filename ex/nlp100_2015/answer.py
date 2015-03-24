#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import argparse

def p000():
    """
    00. 文字列の逆順
    文字列"stressed"の文字を逆に（末尾から先頭に向かって）並べた文字列を得よ．
    """
    print 'stressed'[::-1]

def main():
    default_number = 0
    parser = argparse.ArgumentParser(description='言語処理100本ノック2015')
    parser.add_argument('-n', '--number', nargs='?', default=default_number, type=int, help='問題の番号')
    args = parser.parse_args()

    func = globals()['p' + '%03d' % args.number]
    func()

if __name__ == '__main__':
    main()

