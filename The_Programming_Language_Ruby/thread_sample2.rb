require 'thread'

# Mutex
lock = Mutex.new

# 1つ目のスレッド（作成後、すぐに走り始める）
Thread.new do
  5.times do
    lock.synchronize { puts "Thread1"; $stdout.flush }
  end
end

# 2つ目のスレッド（作成後、すぐに走り始める）
Thread.new do
  5.times do
    lock.synchronize { puts "Thread2"; $stdout.flush }
  end
end

# 上記2つのスレッドの実行が終わるのを待つ
Thread.list.each { |t| t.join unless t == Thread.current }

puts "Main Thread"

