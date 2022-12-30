
all: A

A:
	cd toolkit; ./judge.sh generator/testcase.txt visualizer/default.json python3 ../main.py

B:
	# TODO

A_default:
	cd toolkit; ./judge.sh generator/testcase_A_default.txt visualizer/default.json python3 ../main.py

A_max:
	cd toolkit; ./judge.sh generator/testcase_A_max.txt visualizer/default.json python3 ../main.py

config:
	cd toolkit/generator; ./random_world.py -c config.toml > testcase.txt 

test:
	python3 -m unittest main_test.py

