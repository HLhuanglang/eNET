.PHONY: test
test:
	mkdir -p ../bin/test
	g++ -std=c++11 util_test.cpp -o ../bin/test/util_test -g -I../output/include	-lpthread
	g++ -std=c++11 event_loop_test.cpp -o ../bin/test/event_loop_test -g -I../output/include  -L../output/lib -leasynet -Wl,-rpath=$(LIB_PATH) -lpthread
	g++ -std=c++11 subreactor_pool_test.cpp -o ../bin/test/subreactor_pool -g -I../output/include  -L../output/lib -leasynet -Wl,-rpath=$(LIB_PATH) -lpthread
	g++ -std=c++11 ini_test.cpp -o ../bin/test/ini -g -I../output/include  -L../output/lib -leasynet -Wl,-rpath=$(LIB_PATH) -lpthread
	g++ -std=c++11 log_test.cpp -o ../bin/test/log -g -I../output/include  -L../output/lib -leasynet -Wl,-rpath=$(LIB_PATH) -lpthread
	g++ -std=c++11 buffer_test.cpp -o ../bin/test/buffer_test -g -I../output/include  -L../output/lib -leasynet -Wl,-rpath=$(LIB_PATH) -lpthread
	g++ -std=c++11 tcp_server_test.cpp -o ../bin/test/tcp_server_test -g -I../output/include  -L../output/lib -leasynet -Wl,-rpath=$(LIB_PATH) -lpthread