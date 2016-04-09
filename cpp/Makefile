all:echo_reverse_worker echo_reverse_client echo_background_client gearman_bench

echo_reverse_worker:echo_reverse_worker.cpp
	g++ -std=c++0x -o echo_reverse_worker echo_reverse_worker.cpp -I/usr/local/gearman/include -L/usr/local/gearman/lib -lgearman

echo_reverse_client:echo_reverse_client.cpp
	g++ -std=c++0x -o echo_reverse_client echo_reverse_client.cpp -I/usr/local/gearman/include -L/usr/local/gearman/lib -lgearman

echo_background_client:echo_background_client.cpp
	g++ -std=c++0x -o echo_background_client echo_background_client.cpp -I/usr/local/gearman/include -L/usr/local/gearman/lib -lgearman

gearman_bench:gearman_bench.cpp
	g++ -std=c++0x -o gearman_bench gearman_bench.cpp -I/usr/local/gearman/include -L/usr/local/gearman/lib -lgearman

clean:echo_reverse_worker echo_reverse_client echo_background_client gearman_bench
	- rm echo_reverse_worker echo_reverse_client echo_background_client gearman_bench -f
