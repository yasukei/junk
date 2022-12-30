
all: A

A:
	cd toolkit; ./judge.sh generator/testcase.txt visualizer/default.json python3 ../main.py

B:
	# TODO

config:
	cd toolkit/generator; ./random_world.py -c config.toml > testcase.txt 

test:
	python3 -m unittest main_test.py

