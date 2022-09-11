target=libeasynet.so

.PHONY: all
all:
	make raw_examples build test 

.PHONY: build
build:
	mkdir -p output/lib output/include
	g++ -g -fPIC -shared ./net/*.cpp ./util/*.cpp -o ${target}
	mv -u ${target} output/lib
	cp ./net/*.h output/include
	cp ./util/*.h output/include

.PHONY: test
test:
	make -C test

.PHONY: raw_examples
raw_examples:
	make -C raw_examples

.PHONY: clean
clean:
	rm -rf output
	rm -rf bin