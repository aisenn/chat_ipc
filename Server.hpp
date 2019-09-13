#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>

#include "Client.hpp"

#define PORT	8888
#define BUFSIZE 1024

class Server {
    fd_set      _master;
    fd_set      _readfds;
    int         _fdmax;
    int         _sockfd;
    sockaddr_in _address;
	bool		_exit;
	std::vector<std::unique_ptr<Client>> _clients;

public:
	Server();
	~Server();

	void run();
	void newConnectionAccept();
	std::string signin(int fd);
	void sendMsgToClients(int clientSocket, std::string &message);
	void broadcast(Client &client);
	void sendInfo();
};

#endif //SERVER_HPP
