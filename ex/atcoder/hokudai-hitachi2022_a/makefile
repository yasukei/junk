
all: A_default

A:
	cd toolkit; ./judge.sh generator/testcase.txt visualizer/default.json python3 ../main.py

B:
	# TODO

A_default:
	cd toolkit; ./judge.sh generator/testcase_A_default.txt visualizer/default.json python3 ../main.py
	#cd toolkit; ./judge.sh generator/testcase_A_default.txt visualizer/default.json python3 ../A_DQN.py

A_max:
	cd toolkit; ./judge.sh generator/testcase_A_max.txt visualizer/default.json python3 ../main.py

profile:
	cd toolkit; python3 ../main.py < debug_input.txt | tee debug_output.txt

config:
	cd toolkit/generator; ./random_world.py -c config.toml > testcase.txt 

graph:
	cd toolkit; dot graph.dot -T png -o graph.png

test:
	python3 -m unittest main_test.py

