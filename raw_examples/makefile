.PHONY: demo
demo:
	mkdir -p ../bin/raw_examples
	g++ -std=c++11 epoll_demo.cpp -o 	../bin/raw_examples/epoll_demo -g
	g++ -std=c++11 timerfd_demo.cpp -o 	../bin/raw_examples/timer_demo -g
	g++ -std=c++11 eventfd_demo.cpp -o 	../bin/raw_examples/eventfd_demo -g -lpthread