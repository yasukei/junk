mkdir out
for num in $(seq --format '%04g' 0 99); do
	./main < "./in/$num.txt" > "./out/$num.txt" 
done
