server: libeventserv.cpp
	g++ -L/usr/lib/mysql -lmysqlclient -levent -lpthread -levent_pthreads libeventserv.cpp -o libserver
	g++ myclient.c.cpp -o client
