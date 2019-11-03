BUILD_DIR:=build

.PHONY: all build test run lcov clean rebuild

all: test

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make

test: build
	cd $(BUILD_DIR) && ctest .. --verbose

run: test

lcov: test
	cd $(BUILD_DIR)/test \
		&& lcov -d . -c -o coverage.info \
		&& lcov -r coverage.info */googletest/* test/* */c++/* -o coverage_filtered.info \
		&& genhtml -o lcov.html coverage_filtered.info

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

