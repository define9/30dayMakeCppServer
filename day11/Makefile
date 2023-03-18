server:
	g++ src/util.cpp src/Channel.cpp client.cpp src/Buffer.cpp src/Connection.cpp src/Socket.cpp src/InetAddress.cpp -o client && \
	g++ src/util.cpp -pthread src/ThreadPool.cpp server.cpp src/Buffer.cpp src/Connection.cpp src/Acceptor.cpp src/Epoll.h src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Channel.cpp src/Server.cpp src/EventLoop.cpp -o server
clean:
	rm server && rm client
threadTest:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest

test:
	g++ -pthread src/ThreadPool.cpp src/util.cpp test.cpp -o test