all:
	g++ -o cansocket_test cansocket_test.cpp -g -Wall
	g++ -o cansocket_bench cansocket_bench.cpp -g -Wall -pthread
	g++ -o cansocket_dura cansocket_dura.cpp -g -Wall -pthread

clean:
	sudo ifconfig can0 down
	sudo ifconfig can1 down
	sudo ifconfig can0 up
	sudo ifconfig can1 up
