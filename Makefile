all:
	g++ test.cpp -o test -lsimlib -lm
	./test
run:
	./test