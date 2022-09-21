.PHONY: demo
demo:
	mkdir -p ../../bin/raw_examples/libuv
	g++ -std=c++11 echo_server.cpp -o ../../bin/raw_examples/libuv/echo_server -lpthread -I/opt/libuv/include -L/opt/libuv/lib -luv

.PHONY: clean
clean:
	rm -rf echo_server