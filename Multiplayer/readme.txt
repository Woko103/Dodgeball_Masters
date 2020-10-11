Made with Boost Asio.

If you want to launch this server, you need to use:

./server initial_port your_IP end_port

Example: ./server 12345 83.192.1.1 50000


If you need to compile it, you can use:

g++ async_server.cpp -o server -L/../boost/lib -lboost_filesystem -lboost_system -lpthread


More info:

- The multiplayer of this game hasn't been tested correctly, so it's possible that
contains bugs.

- The mode with the least bugs is the 1vs1. Only works wrong if you lose packages on the conection.