all: test #ctest

# This compiles to Intel Cilk.
test: test.cpp
	clang++ -DUSE_CILK -lpthread -fcilkplus -O3 test.cpp -o test

# This compiles to Cheetah. You should have libcheetah in this directory and rename it to cilkr.so.
#ctest: test.cpp
#	clang++ -L. -lpthread -ftapir=cilkr  -O3 test.cpp -o ctest

clean:
	rm -f test ctest
