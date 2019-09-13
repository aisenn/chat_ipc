#include "Client.hpp"

Client::Client() : _name(), _socket(), _clientAddr() {}

const std::string &Client::getName() const {
	return _name;
}

int Client::getSocket() const {
	return _socket;
}

const sockaddr_in &Client::getClientAddr() const {
	return _clientAddr;
}

void Client::setName(const std::string &name) {
	Client::_name = name;
}

void Client::setSocket(int socket) {
	Client::_socket = socket;
}
