#pragma once

#include "../includes/Includes.hpp"
#include "../includes/Helper.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Commands.hpp"

class Commands;
class Server
{
private:
	int _socket;
	Commands *cmds;
	const int _port;
	const std::string _password;
	std::map<int, Client> clients;
	std::map<std::string, Channel> channels;
	struct sockaddr_in _ServerAddress;
	struct sockaddr_in _ClientAddress;
	socklen_t _ClientAddrLen;

	static const int _maxConnections = 128;
	static bool Signal;
	static void SignalHandler(int signum);

	void srvInit();
	void srvLstn();
	void Recv();
	void Check();
	void Remv();

public:
	std::map<int, Client> &getClients();
	std::map<std::string, Channel> &getChannels();
	std::string getPassword();
	bool getSignal();
	void Send(int _clientSocket, const std::string &message);
	Server(int port, std::string password);
	~Server();
	void start();
};
