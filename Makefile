server: 
	g++ -L/usr/lib/mysql -lmysqlclient -levent -lpthread -levent_pthreads server.cpp -o server
	g++ client.cpp -o client
