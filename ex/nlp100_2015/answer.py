#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import string
import argparse
import codecs
from collections import Counter
from collections import OrderedDict

def p000():
    """
    00. 文字列の逆順
    文字列"stressed"の文字を逆に（末尾から先頭に向かって）並べた文字列を得よ．
    """
    print 'stressed'[::-1]

def p001():
    """
    01. 「パタトクカシーー」
    「パタトクカシーー」という文字列の1,3,5,7文字目を取り出して連結した文字列を得よ．
    """
    sys.stdout = codecs.getwriter('utf_8')(sys.stdout)
    print u"パタトクカシーー"[::2]

def p002():
    """
    02. 「パトカー」＋「タクシー」＝「パタトクカシーー」
    「パトカー」＋「タクシー」の文字を先頭から交互に連結して文字列「パタトクカシーー」を得よ．
    """
    sys.stdout = codecs.getwriter('utf_8')(sys.stdout)
    x = u'パトカー'
    y = u'タクシー'
    z = u''
    for i in range(0, max(len(x), len(y))):
        z += x[i] + y[i]    # FIXME: might cause IndexError
    print z

def p003():
    """
    03. 円周率
    "Now I need a drink, alcoholic of course, after the heavy lectures involving quantum mechanics."という文を単語に分解し，各単語の（アルファベットの）文字数を先頭から出現順に並べたリストを作成せよ．
    """
    sentence = "Now I need a drink, alcoholic of course, after the heavy lectures involving quantum mechanics."
    sentence = string.lower(sentence)
    chars = [sentence[i:i+1] for i in range(0, len(sentence))]
    alphabets = [char for char in chars if char in string.lowercase]
    counter = Counter()
    for alpha in alphabets:
        counter[alpha] += 1
    ordered = OrderedDict(sorted(counter.items(), key=lambda t: t[1], reverse=True))
    for key, value in ordered.items():
        print str(key) + ', ' + str(value)

def p004():
    """
    04. 元素記号
    "Hi He Lied Because Boron Could Not Oxidize Fluorine. New Nations Might Also Sign Peace Security Clause. Arthur King Can."という文を単語に分解し，1, 5, 6, 7, 8, 9, 15, 16, 19番目の単語は先頭の1文字，それ以外の単語は先頭に2文字を取り出し，取り出した文字列から単語の位置（先頭から何番目の単語か）への連想配列（辞書型もしくはマップ型）を作成せよ．
    """
    sentence = "Hi He Lied Because Boron Could Not Oxidize Fluorine. New Nations Might Also Sign Peace Security Clause. Arthur King Can."
    words = sentence.split()
    need_only_one = [1, 5, 6, 7, 8, 9, 15, 16, 19]
    table = OrderedDict()
    for i, word in enumerate(words):
        ith = i + 1
        if ith in need_only_one:
            chars = word[:1]
        else:
            chars = word[:2]
        table[chars] = ith
    print table

def p005():
    """
    05. n-gram
    与えられたシーケンス（文字列やリストなど）からn-gramを作る関数を作成せよ．この関数を用い，"I am an NLPer"という文から単語bi-gram，文字bi-gramを得よ．
    """
    sentence = "I am an NLPer"
    word_bi_gram = make_n_gram(sentence, 2, 'word')
    char_bi_gram = make_n_gram(sentence, 2, 'char')
    print word_bi_gram
    print char_bi_gram

def make_n_gram(sentence, n=2, type='word'):
    if type == 'word':
        elems = sentence.split()
        func = lambda x, y: x + ' ' + y
    elif type == 'char':
        elems = [sentence[i:i+1] for i in range(0, len(sentence))]
        func = lambda x, y: x + y
    else:
        return None

    n_gram = []
    for i in range(0, len(elems)):
        n_gram.append(reduce(func, elems[:n]))
        elems = elems[1:]
    return n_gram

def p006():
    """
    06. 集合
    "paraparaparadise"と"paragraph"に含まれる文字bi-gramの集合を，それぞれ, XとYとして求め，XとYの和集合，積集合，差集合を求めよ．さらに，'se'というbi-gramがXおよびYに含まれるかどうかを調べよ．
    """
    X = set(make_n_gram("paraparaparadise", 2, 'char'))
    Y = set(make_n_gram("paragraph", 2, 'char'))
    # 和集合
    print X | Y
    # 積集合
    print X & Y
    # 差集合
    print X - Y
    
    if 'se' in X:   print 'include'
    else:           print 'not include'

    if 'se' in Y:   print 'include'
    else:           print 'not include'

def p007():
    """
    07. テンプレートによる文生成
    引数x, y, zを受け取り「x時のyはz」という文字列を返す関数を実装せよ．さらに，x=12, y="気温", z=22.4として，実行結果を確認せよ．
    """
    pass


def main():
    default_number = 6
    parser = argparse.ArgumentParser(description='言語処理100本ノック2015')
    parser.add_argument('-n', '--number', nargs='?', default=default_number, type=int, help='問題の番号')
    args = parser.parse_args()

    func = globals()['p' + '%03d' % args.number]
    func()

if __name__ == '__main__':
    main()

"""
08. 暗号文
与えられた文字列の各文字を，以下の仕様で変換する関数cipherを実装せよ．

英小文字ならば(219 - 文字コード)の文字に置換
その他の文字はそのまま出力
この関数を用い，英語のメッセージを暗号化・復号化せよ．

09. Typoglycemia
スペースで区切られた単語列に対して，各単語の先頭と末尾の文字は残し，それ以外の文字の順序をランダムに並び替えるプログラムを作成せよ．ただし，長さが４以下の単語は並び替えないこととする．適当な英語の文（例えば"I couldn't believe that I could actually understand what I was reading : the phenomenal power of the human mind ."）を与え，その実行結果を確認せよ．
"""
