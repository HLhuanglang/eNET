.PHONY: test
test:
	mkdir -p ../bin/test
	g++ -std=c++11 util_test.cpp -o ../bin/test/util_test -g -I../net	-lpthread
	g++ -std=c++11 event_loop_test.cpp -o ../bin/test/event_loop_test -g -I../net -I../util -L../output/lib -leasynet -Wl,-rpath=/home/ubuntu/code/mycpp/EasyNet/output/lib -lpthread
	g++ -std=c++11 subreactor_pool_test.cpp -o ../bin/test/subreactor_pool -g -I../net -I../util -L../output/lib -leasynet -Wl,-rpath=/home/ubuntu/code/mycpp/EasyNet/output/lib -lpthread
	g++ -std=c++11 ini_test.cpp -o ../bin/test/ini -g -I../net -I../util -L../output/lib -leasynet -Wl,-rpath=/home/ubuntu/code/mycpp/EasyNet/output/lib -lpthread
	g++ -std=c++11 log_test.cpp -o ../bin/test/log -g -I../net -I../util -L../output/lib -leasynet -Wl,-rpath=/home/ubuntu/code/mycpp/EasyNet/output/lib -lpthread