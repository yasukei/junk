#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import string
import argparse
import codecs
from collections import Counter
from collections import OrderedDict
from string import Template
import random
import subprocess
import json
import re

"""
第1章: 準備運動
"""

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
    print u"パタトクカシーー"[::2]

def p002():
    """
    02. 「パトカー」＋「タクシー」＝「パタトクカシーー」
    「パトカー」＋「タクシー」の文字を先頭から交互に連結して文字列「パタトクカシーー」を得よ．
    """
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
    chars = str_to_chars(sentence)
    alphabets = [char for char in chars if char in string.lowercase]
    counter = Counter()
    for alpha in alphabets:
        counter[alpha] += 1
    ordered = OrderedDict(sorted(counter.items(), key=lambda t: t[1], reverse=True))
    for key, value in ordered.items():
        print str(key) + ', ' + str(value)

def str_to_chars(str):
    return [str[i:i+1] for i in range(0, len(str))]

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
        elems = str_to_chars(sentence)
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
    def make_string(x, y, z):
        s = Template(u'${x}時の${y}は${z}')
        return s.safe_substitute({ 'x' : x, 'y' : y, 'z' : z})

    s = make_string(12, u"気温", 22.4)
    print s

def p008():
    """
    08. 暗号文
    与えられた文字列の各文字を，以下の仕様で変換する関数cipherを実装せよ．

    英小文字ならば(219 - 文字コード)の文字に置換
    その他の文字はそのまま出力
    この関数を用い，英語のメッセージを暗号化・復号化せよ．
    """
    def cipher(message):
        chars = str_to_chars(message)
        ciphered = ''
        for c in chars:
            if c in string.ascii_lowercase:
                ciphered += chr(219 - ord(c))
            else:
                ciphered += c
        return ciphered

    print cipher('Hello world!')
                
def p009():
    """
    09. Typoglycemia
    スペースで区切られた単語列に対して，各単語の先頭と末尾の文字は残し，それ以外の文字の順序をランダムに並び替えるプログラムを作成せよ．ただし，長さが４以下の単語は並び替えないこととする．適当な英語の文（例えば"I couldn't believe that I could actually understand what I was reading : the phenomenal power of the human mind ."）を与え，その実行結果を確認せよ．
    """
    sentence = "I couldn't believe that I could actually understand what I was reading : the phenomenal power of the human mind ."
    words = sentence.split()
    typoglycemia = []
    for word in words:
        if len(word) <= 4:
            typoglycemia.append(word)
        else:
            chars = str_to_chars(word[1:-1])
            s = ''
            while len(chars) > 0:
                s += chars.pop(random.randint(0, len(chars) - 1))
            typoglycemia.append(word[0] + s + word[-1])
    typoglycemia = ' '.join(typoglycemia)
    print typoglycemia


"""
第2章: UNIXコマンドの基礎

hightemp.txtは，日本の最高気温の記録を「都道府県」「地点」「℃」「日」のタブ区切り形式で格納したファイルである．以下の処理を行うプログラムを作成し，hightemp.txtを入力ファイルとして実行せよ．さらに，同様の処理をUNIXコマンドでも実行し，プログラムの実行結果を確認せよ．
"""

def p010():
    """
    10. 行数のカウント
    行数をカウントせよ．確認にはwcコマンドを用いよ．
    """
    subprocess.call(['wc', '-l', 'hightemp.txt'])

    with open('hightemp.txt', 'r') as f:
        lines = f.readlines()
        print str(len(lines))

def p011():
    """
    11. タブをスペースに置換
    タブ1文字につきスペース1文字に置換せよ．確認にはsedコマンド，trコマンド，もしくはexpandコマンドを用いよ．
    """
    subprocess.call(['sed', '-e', 's/\t/ /g', 'hightemp.txt'])

    with open('hightemp.txt', 'r') as f:
        lines = f.readlines()
        print ''.join([line.replace('\t', ' ') for line in lines])

def p012():
    """
    12. 1列目をcol1.txtに，2列目をcol2.txtに保存
    各行の1列目だけを抜き出したものをcol1.txtに，2列目だけを抜き出したものをcol2.txtとしてファイルに保存せよ．確認にはcutコマンドを用いよ．
    """
    subprocess.call(['cut', '-f1', 'hightemp.txt'])
    subprocess.call(['cut', '-f2', 'hightemp.txt'])

    lines = []
    with open('hightemp.txt', 'r') as f:
        lines = f.readlines()

    with open('col1.txt', 'w') as f:
        for line in lines:
            f.write(line.split('\t')[0] + '\n')
    with open('col2.txt', 'w') as f:
        for line in lines:
            f.write(line.split('\t')[1] + '\n')

def p013():
    """
    13. col1.txtとcol2.txtをマージ
    12で作ったcol1.txtとcol2.txtを結合し，元のファイルの1列目と2列目をタブ区切りで並べたテキストファイルを作成せよ．確認にはpasteコマンドを用いよ．
    """
    subprocess.call(['paste', 'col1.txt', 'col2.txt'])

    col1_lines = []
    with open('col1.txt', 'r') as f:
        for line in f.readlines():
            col1_lines.append(line.strip('\n'))

    col2_lines = []
    with open('col2.txt', 'r') as f:
        for line in f.readlines():
            col2_lines.append(line.strip('\n'))

    with open('col1_col2.txt', 'w') as f:
        for i in range(0, max(len(col1_lines), len(col2_lines))):
            if i < len(col1_lines):
                f.write(col1_lines[i])
            f.write('\t')
            if i < len(col2_lines):
                f.write(col2_lines[i])
            f.write('\n')

def p014():
    """
    14. 先頭からN行を出力
    自然数Nをコマンドライン引数などの手段で受け取り，入力のうち先頭のN行だけを表示せよ．確認にはheadコマンドを用いよ．
    """
    N = 3

    subprocess.call(['head', '-n' + str(N), 'hightemp.txt'])

    with open('hightemp.txt', 'r') as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        if i >= N:
            break
        print line,

def p015():
    """
    15. 末尾のN行を出力
    自然数Nをコマンドライン引数などの手段で受け取り，入力のうち末尾のN行だけを表示せよ．確認にはtailコマンドを用いよ．
    """
    N = 3

    subprocess.call(['tail', '-n' + str(N), 'hightemp.txt'])

    with open('hightemp.txt', 'r') as f:
        lines = f.readlines()

    if N < len(lines):
        start = len(lines) - N
    else:
        start = 0
    for i in range(start, len(lines)):
        print lines[i],

def p016():
    """
    16. ファイルをN分割する
    自然数Nをコマンドライン引数などの手段で受け取り，入力のファイルを行単位でN分割せよ．同様の処理をsplitコマンドで実現せよ．
    """
    N = 5

    pipe1 = subprocess.Popen(['wc', '-l', 'hightemp.txt'], stdout=subprocess.PIPE)
    pipe2 = subprocess.Popen(['cut', '-f1', '-d '], stdin=pipe1.stdout, stdout=subprocess.PIPE)
    line_number = int(pipe2.stdout.readline())
    division = line_number / N + 1
    subprocess.call(['split', '-l' + str(division), '-d', 'hightemp.txt', 'hightemp.txt.'])

    lines = []
    with open('hightemp.txt', 'r') as f:
        for line in f.readlines():
            lines.append(line.decode('utf-8'))
    division = len(lines) / N + 1
    files = []
    for i in range(0, division):
        f = lines[i*division:(i+1)*division]
        files.append(f)
    for f in files:
        for line in f:
            print line,
        print

def p017():
    """
    17. １列目の文字列の異なり
    1列目の文字列の種類（異なる文字列の集合）を求めよ．確認にはsort, uniqコマンドを用いよ．
    """
    pipe1 = subprocess.Popen(['cut', '-f1', 'hightemp.txt'], stdout=subprocess.PIPE)
    pipe2 = subprocess.Popen(['sort'], stdin=pipe1.stdout, stdout=subprocess.PIPE)
    subprocess.Popen(['uniq', '-c'], stdin=pipe2.stdout)

    lines = []
    with open('hightemp.txt', 'r') as f:
        for line in f.readlines():
            lines.append(line.decode('utf-8'))
    col1 = set()
    delimiter = '\t'
    for line in lines:
        col1.add(line.split(delimiter)[0])
    for elem in col1:
        print elem

def p018():
    """
    18. 各行を3コラム目の数値の降順にソート
    各行を3コラム目の数値の逆順で整列せよ（注意: 各行の内容は変更せずに並び替えよ）．確認にはsortコマンドを用いよ（この問題はコマンドで実行した時の結果と合わなくてもよい）．
    """
    subprocess.Popen(['sort', '-k3,3', '-r', 'hightemp.txt'])

    matrix = []
    delimiter = '\t'
    with open('hightemp.txt', 'r') as f:
        for line in f.readlines():
            matrix.append(line.decode('utf-8').split(delimiter))

    matrix.sort(key=lambda row: row[2], reverse=True)
    for row in matrix:
        print delimiter.join(row),

def p019():
    """
    19. 各行の1コラム目の文字列の出現頻度を求め，出現頻度の高い順に並べる
    各行の1列目の文字列の出現頻度を求め，その高い順に並べて表示せよ．確認にはcut, uniq, sortコマンドを用いよ．
    """
    pipe1 = subprocess.Popen(['cut', '-f1', 'hightemp.txt'], stdout=subprocess.PIPE)
    pipe2 = subprocess.Popen(['sort'], stdin=pipe1.stdout, stdout=subprocess.PIPE)
    pipe3 = subprocess.Popen(['uniq', '-c'], stdin=pipe2.stdout, stdout=subprocess.PIPE)
    subprocess.Popen(['sort', '-r'], stdin=pipe3.stdout)

    lines = []
    with open('hightemp.txt', 'r') as f:
        for line in f.readlines():
            lines.append(line.decode('utf-8'))

    counter = Counter()
    delimiter = '\t'
    for line in lines:
        counter[line.split(delimiter)[0]] += 1
    ordered = OrderedDict(sorted(counter.items(), key=lambda t: t[1], reverse=True))
    for key, value in ordered.items():
        print key + u', ' + unicode(value)

"""
第3章: 正規表現
Wikipediaの記事を以下のフォーマットで書き出したファイルjawiki-country.json.gzがある．

1行に1記事の情報がJSON形式で格納される
各行には記事名が"title"キーに，記事本文が"text"キーの辞書オブジェクトに格納され，そのオブジェクトがJSON形式で書き出される
ファイル全体はgzipで圧縮される
以下の処理を行うプログラムを作成せよ．
"""

"""
20. JSONデータの読み込み
Wikipedia記事のJSONファイルを読み込み，「イギリス」に関する記事本文を表示せよ．問題21-29では，ここで抽出した記事本文に対して実行せよ．
"""
def p020():
    uk = get_json_by_title_from_file(u'イギリス', 'jawiki-country.json')
    print uk['text']

def parse_json_string(string):
    """
    refer: http://stackoverflow.com/questions/26620714/json-loads-valueerror-extra-data-in-python
    """
    decoder = json.JSONDecoder()
    while string:
        obj, idx = decoder.raw_decode(string)
        yield obj
        string = string[idx:].lstrip()

def get_json_by_title_from_file(title, filepath):
    with open(filepath, 'r') as f:
        for j in parse_json_string(f.read()):
            if j['title'] == title:
                return j
    return None

"""
21. カテゴリ名を含む行を抽出
記事中でカテゴリ名を宣言している行を抽出せよ．
"""
def p021():
    uk = get_json_by_title_from_file(u'イギリス', 'jawiki-country.json')

    category_lines = []
    for line in uk['text'].split('\n'):
        if 'category' in line.lower():
            category_lines.append(line)
            print line

"""
22. カテゴリ名の抽出
記事のカテゴリ名を（行単位ではなく名前で）抽出せよ．
"""
def p022():
    uk = get_json_by_title_from_file(u'イギリス', 'jawiki-country.json')

    category_names = re.findall(r'\[\[Category:(.*)\]\]', uk['text'])

    for category_name in category_names:
        print category_name

"""
23. セクション構造
記事中に含まれるセクション名とそのレベル（例えば"== セクション名 =="なら1）を表示せよ．
"""
def p023():
    pass

"""
24. ファイル参照の抽出
記事から参照されているメディアファイルをすべて抜き出せ．
"""
def p024():
    pass

"""
25. テンプレートの抽出
記事中に含まれる「基礎情報」テンプレートのフィールド名と値を抽出し，辞書オブジェクトとして格納せよ．
"""
def p025():
    pass

"""
26. 強調マークアップの除去
25の処理時に，テンプレートの値からMediaWikiの強調マークアップ（弱い強調，強調，強い強調のすべて）を除去してテキストに変換せよ（参考: マークアップ早見表）．
"""
def p026():
    pass

"""
27. 内部リンクの除去
26の処理に加えて，テンプレートの値からMediaWikiの内部リンクマークアップを除去し，テキストに変換せよ（参考: マークアップ早見表）．
"""
def p027():
    pass

"""
28. MediaWikiマークアップの除去
27の処理に加えて，テンプレートの値からMediaWikiマークアップを可能な限り除去し，国の基本情報を整形せよ．
"""
def p028():
    pass

"""
29. 国旗画像のURLを取得する
テンプレートの内容を利用し，国旗画像のURLを取得せよ．（ヒント: MediaWiki APIのimageinfoを呼び出して，ファイル参照をURLに変換すればよい）
"""
def p029():
    pass

def main():
    default_number = 22
    parser = argparse.ArgumentParser(description='言語処理100本ノック2015')
    parser.add_argument('-n', '--number', nargs='?', default=default_number, type=int, help='問題の番号')
    args = parser.parse_args()

    sys.stdout = codecs.getwriter('utf_8')(sys.stdout)

    func = globals()['p' + '%03d' % args.number]
    func()

if __name__ == '__main__':
    main()

"""
第4章: 形態素解析
夏目漱石の小説『吾輩は猫である』の文章（neko.txt）をMeCabを使って形態素解析し，その結果をneko.txt.mecabというファイルに保存せよ．このファイルを用いて，以下の問に対応するプログラムを実装せよ．

なお，問題37, 38, 39はmatplotlibもしくはGnuplotを用いるとよい．
"""

"""
30. 形態素解析結果の読み込み
形態素解析結果（neko.txt.mecab）を読み込むプログラムを実装せよ．ただし，各形態素は表層形（surface），基本形（base），品詞（pos），品詞細分類1（pos1）をキーとするマッピング型に格納し，1文を形態素（マッピング型）のリストとして表現せよ．第4章の残りの問題では，ここで作ったプログラムを活用せよ．
"""

"""
31. 動詞
動詞の表層形をすべて抽出せよ．
"""

"""
32. 動詞の原形
動詞の原形をすべて抽出せよ．
"""

"""
33. サ変名詞
サ変接続の名詞をすべて抽出せよ．
"""

"""
34. 「AのB」
2つの名詞が「の」で連結されている名詞句を抽出せよ．
"""

"""
35. 名詞の連接
名詞の連接（連続して出現する名詞）を最長一致で抽出せよ．
"""

"""
36. 単語の出現頻度
文章中に出現する単語とその出現頻度を求め，出現頻度の高い順に並べよ．
"""

"""
37. 頻度上位10語
出現頻度が高い10語とその出現頻度をグラフ（例えば棒グラフなど）で表示せよ．
"""

"""
38. ヒストグラム
単語の出現頻度のヒストグラム（横軸に出現頻度，縦軸に出現頻度をとる単語の種類数を棒グラフで表したもの）を描け．
"""

"""
39. Zipfの法則
単語の出現頻度順位を横軸，その出現頻度を縦軸として，両対数グラフをプロットせよ．
"""

"""
第5章: 係り受け解析

夏目漱石の小説『吾輩は猫である』の文章（neko.txt）をCaboChaを使って係り受け解析し，その結果をneko.txt.cabochaというファイルに保存せよ．このファイルを用いて，以下の問に対応するプログラムを実装せよ．
"""

"""
40. 係り受け解析結果の読み込み（形態素）
形態素を表すクラスMorphを実装せよ．このクラスは表層形（surface），基本形（base），品詞（pos），品詞細分類1（pos1）をメンバ変数に持つこととする．さらに，CaboChaの解析結果（neko.txt.cabocha）を読み込み，各文をMorphオブジェクトのリストとして表現し，3文目の形態素列を表示せよ．
"""

"""
41. 係り受け解析結果の読み込み（文節・係り受け）
40に加えて，文節を表すクラスChunkを実装せよ．このクラスは形態素（Morphオブジェクト）のリスト（morphs），係り先文節インデックス番号（dst），係り元文節インデックス番号のリスト（srcs）をメンバ変数に持つこととする．さらに，入力テキストのCaboChaの解析結果を読み込み，１文をChunkオブジェクトのリストとして表現し，8文目の文節の文字列と係り先を表示せよ．第5章の残りの問題では，ここで作ったプログラムを活用せよ．
"""

"""
42. 係り元と係り先の文節の表示
係り元の文節と係り先の文節のテキストをタブ区切り形式ですべて抽出せよ．ただし，句読点などの記号は出力しないようにせよ．
"""

"""
43. 名詞を含む文節が動詞を含む文節に係るものを抽出
名詞を含む文節が，動詞を含む文節に係るとき，これらをタブ区切り形式で抽出せよ．ただし，句読点などの記号は出力しないようにせよ．
"""

"""
44. 係り受け木の可視化
与えられた文の係り受け木を有向グラフとして可視化せよ．可視化には，係り受け木をDOT言語に変換し，Graphvizを用いるとよい．また，Pythonから有向グラフを直接的に可視化するには，pydotを使うとよい．
"""

"""
45. 動詞の格パターンの抽出
今回用いている文章をコーパスと見なし，日本語の述語が取りうる格を調査したい． 動詞を述語，動詞に係っている文節の助詞を格と考え，述語と格をタブ区切り形式で出力せよ． ただし，出力は以下の仕様を満たすようにせよ．

動詞を含む文節において，最左の動詞の基本形を述語とする
述語に係る助詞を格とする
述語に係る助詞（文節）が複数あるときは，すべての助詞をスペース区切りで辞書順に並べる
「吾輩はここで始めて人間というものを見た」という例文（neko.txt.cabochaの8文目）を考える． この文は「始める」と「見る」の２つの動詞を含み，「始める」に係る文節は「ここで」，「見る」に係る文節は「吾輩は」と「ものを」と解析された場合は，次のような出力になるはずである．

始める  で
見る    は を
このプログラムの出力をファイルに保存し，以下の事項をUNIXコマンドを用いて確認せよ．

コーパス中で頻出する述語と格パターンの組み合わせ
「する」「見る」「与える」という動詞の格パターン（コーパス中で出現頻度の高い順に並べよ）
"""

"""
46. 動詞の格フレーム情報の抽出
45のプログラムを改変し，述語と格パターンに続けて項（述語に係っている文節そのもの）をタブ区切り形式で出力せよ．45の仕様に加えて，以下の仕様を満たすようにせよ．

項は述語に係っている文節の単語列とする（末尾の助詞を取り除く必要はない）
述語に係る文節が複数あるときは，助詞と同一の基準・順序でスペース区切りで並べる
「吾輩はここで始めて人間というものを見た」という例文（neko.txt.cabochaの8文目）を考える． この文は「始める」と「見る」の２つの動詞を含み，「始める」に係る文節は「ここで」，「見る」に係る文節は「吾輩は」と「ものを」と解析された場合は，次のような出力になるはずである．

始める  で      ここで
見る    は を   吾輩は ものを
"""

"""
47. 機能動詞構文のマイニング
動詞のヲ格にサ変接続名詞が入っている場合のみに着目したい．46のプログラムを以下の仕様を満たすように改変せよ．

「サ変接続名詞+を（助詞）」で構成される文節が動詞に係る場合のみを対象とする
述語は「サ変接続名詞+を+動詞の基本形」とし，文節中に複数の動詞があるときは，最左の動詞を用いる
述語に係る助詞（文節）が複数あるときは，すべての助詞をスペース区切りで辞書順に並べる
述語に係る文節が複数ある場合は，すべての項をスペース区切りで並べる（助詞の並び順と揃えよ）
例えば「別段くるにも及ばんさと、主人は手紙に返事をする。」という文から，以下の出力が得られるはずである．

返事をする      と に は        及ばんさと 手紙に 主人は
このプログラムの出力をファイルに保存し，以下の事項をUNIXコマンドを用いて確認せよ．

コーパス中で頻出する述語（サ変接続名詞+を+動詞）
コーパス中で頻出する述語と助詞パターン
"""

"""
48. 名詞から根へのパスの抽出
文中のすべての名詞を含む文節に対し，その文節から構文木の根に至るパスを抽出せよ． ただし，構文木上のパスは以下の仕様を満たすものとする．

各文節は（表層形の）形態素列で表現する
パスの開始文節から終了文節に至るまで，各文節の表現を"->"で連結する
「吾輩はここで始めて人間というものを見た」という文（neko.txt.cabochaの8文目）から，次のような出力が得られるはずである．

吾輩は -> 見た
ここで -> 始めて -> 人間という -> ものを -> 見た
人間という -> ものを -> 見た
ものを -> 見た
"""

"""
49. 名詞間の係り受けパスの抽出
文中のすべての名詞句のペアを結ぶ最短係り受けパスを抽出せよ．ただし，名詞句ペアの文節番号がiとj（i<j）のとき，係り受けパスは以下の仕様を満たすものとする．

問題48と同様に，パスは開始文節から終了文節に至るまでの各文節の表現（表層形の形態素列）を"->"で連結して表現する
文節iとjに含まれる名詞句はそれぞれ，XとYに置換する
また，係り受けパスの形状は，以下の2通りが考えられる．

文節iから構文木の根に至る経路上に文節jが存在する場合: 文節iから文節jのパスを表示
上記以外で，文節iと文節jから構文木の根に至る経路上で共通の文節kで交わる場合: 文節iから文節kに至る直前のパスと文節jから文節kに至る直前までのパス，文節kの内容を"|"で連結して表示
例えば，「吾輩はここで始めて人間というものを見た。」という文（neko.txt.cabochaの8文目）から，次のような出力が得られるはずである．

Xは | Yで -> 始めて -> 人間という -> ものを | 見た
Xは | Yという -> ものを | 見た
Xは | Yを | 見た
Xで -> 始めて -> Y
Xで -> 始めて -> 人間という -> Y
Xという -> Y
"""

