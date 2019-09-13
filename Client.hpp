#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <netdb.h>

class Client {
private:
	std::string _name;
	int			_socket;
	sockaddr_in _clientAddr;

public:
	Client();

	void setName(const std::string &name);
	void setSocket(int socket);

	const std::string &getName() const;
	int getSocket() const;
	const sockaddr_in &getClientAddr() const;
};

#endif //CLIENT_HPP
