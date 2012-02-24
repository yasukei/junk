#!/usr/bin/ruby

# 4.
# 問題3をまじめに考えると、nより小さいk個の重複のない
# 整数をどのように生成するかという問題に直面する。
# 0からn-1までの間のk個の重複のない整数を生成するには
# どうしたらよいでしょう。
# 速くて短いプログラムを作ってください。

# 素朴に思いつくまま作った
# n=1,000,000 k=10,000で、real 1m5s

# Resouces and Settings
n = 10**6
k = 10**4
fname = "num_list.txt"

array = Array.new(n)
(0..n-1).each do |n|
  array[n] = n
end

File.open(fname, "wb") do |file|
  k.times do
    i = Integer(array.length * rand)
    file << array[i].to_s + "\n"
    array.delete_at(i)
  end
end

