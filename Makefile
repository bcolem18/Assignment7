all: hello-cpp-world hello-c-world Prelim.cpp Numerical.cpp NumMaster.cpp

%: %.cc
	g++ -std=c++11 $< -o $@

%: %.c
	gcc $< -o $@

