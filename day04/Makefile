server:
	g++ util.cpp client.cpp -o client && \
	g++ util.cpp server.cpp Epoll.cpp Epoll.h InetAddress.cpp Socket.cpp -o server
clean:
	rm server && rm client