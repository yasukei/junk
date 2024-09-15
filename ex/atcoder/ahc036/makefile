
SEQ:=`seq -f "%04g" 0 9`
#SEQ:=`seq -f "%04g" 0 99`

all:
	@mkdir -p tools/out
	cd tools ; \
	for i in ${SEQ} ; do \
		echo "in/$${i}.txt" ; \
		python3 ../main_v8.py < "in/$${i}.txt" > "out/$${i}.txt" ; \
		cargo run -r --bin vis "in/$${i}.txt" "out/$${i}.txt" 2>&1 | grep "Score" ; \
	done
