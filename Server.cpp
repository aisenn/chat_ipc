#include "Server.hpp"
#include <cstring>

Server::Server( ) : _master(), _readfds(), _fdmax(), _sockfd(), _address(),
			_exit(false) {
	FD_ZERO(&_master);
	FD_ZERO(&_readfds);

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(PORT);

	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	int mulConnection = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &mulConnection, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (bind(_sockfd, (struct sockaddr *)&_address, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(_sockfd, SOMAXCONN) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	FD_SET(_sockfd, &_master);
	_fdmax = _sockfd;
}

void Server::run()
{
	std::cout << "Waiting for connections on port " << PORT << std::endl;

	while(!_exit) {
		_readfds = _master;
		if(select(_fdmax + 1, &_readfds, nullptr, nullptr, nullptr) == -1){
			perror("select");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(_sockfd, &_readfds))
			newConnectionAccept();
		else {
			for (auto &it : _clients) {
				if (FD_ISSET(it->getSocket(), &_readfds)) {
					broadcast(*it);
					break;
				}
			}
		}
	}
}

void Server::newConnectionAccept() {
	_clients.emplace_back(std::make_unique<Client>());

	auto		&clienPtr = _clients.back();
	socklen_t   addrlen;
	int         newClient;

	addrlen = sizeof(struct sockaddr_in);
	if( (newClient = accept(_sockfd, (struct sockaddr *)&clienPtr->getClientAddr(), &addrlen)) == -1) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
	else {
		FD_SET(newClient, &_master);

		if(_fdmax < newClient)
			_fdmax = newClient;

		clienPtr->setName( signin(newClient) );
		clienPtr->setSocket(newClient);
		sendInfo();
	}
}

std::string Server::signin(int fd) {
	std::string message = "Welcome to the chat.\nEnter your name please: ";
	std::string name;

	while (name.empty())
	{
		if (send(fd, message.c_str(), message.size() + 1, 0) == -1)
			perror("send");

		char buf[BUFSIZE];
		memset(buf, 0, BUFSIZE);
		if (recv(fd, buf, BUFSIZE, 0) <= 0)
		{
			perror("recv");
			close(fd);
			FD_CLR(fd, &_master);
		}
		else
		{
			std::stringstream ss;
			ss << buf;
			ss >> name;

			if (name.empty())
				message = "Name field can not be empty.\nEnter your name please: ";
		}
	}
	return name;
}

void Server::sendMsgToClients(int clientSocket, std::string &message) {
	int sock;

	for (auto &it : _clients) {
		sock = it->getSocket();
		if (sock != clientSocket) {
			if (FD_ISSET(sock, &_master))
			{
				if (send(sock, message.c_str(), message.size() + 1, 0) == -1) {
					perror("send");
				}
			}
		}
	}
}

void Server::broadcast(Client &client) {
	char	buf[BUFSIZE];
	int		sock = client.getSocket();

	memset(buf, 0, BUFSIZE);
	if (recv(sock, buf, BUFSIZE, 0) <= 0) {
		perror("recv");
		close(sock);
		FD_CLR(sock, &_master);
	} else {
		std::ostringstream ss;

		ss << client.getName() << ": " << buf;
		std::string message = ss.str();

		sendMsgToClients(sock, message);
	}
}

void Server::sendInfo() {
	auto &clienPtr = _clients.back();
	std::stringstream ss;

	ss << "Server: " << clienPtr->getName() << " just joined the chat." << std::endl;
	std::string message = ss.str();

	sendMsgToClients(clienPtr->getSocket(), message);
	std::cout << "New connection on port " << clienPtr->getClientAddr().sin_port << std::endl;
}

Server::~Server() {}
