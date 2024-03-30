#include "../../includes/Server.hpp"

bool Server::Signal = false;

Server::Server(int port, std::string password) : _port(port), _password(password), _signal(false), cmds(NULL)
{
	cmds = new Commands(this);
}

Server::~Server()
{
	Server::Signal = true;
	Remv();
	close(_socket);
}

std::map<std::string, Channel> &Server::getChannels()
{
	return (channels);
}

std::map<int, Client> &Server::getClients()
{
	return (clients);
}

std::string Server::getPassword()
{
	return (_password);
}

bool Server::getSignal()
{
	return (_signal);
}

void Server::SignalHandler(int signum)
{
	(void)signum;
	Server::Signal = true;
	std::cout << red << "killed Signal" << res << std::endl;
}

void Server::start()
{
	signal(SIGINT, Server::SignalHandler);
	signal(SIGQUIT, Server::SignalHandler);
	srvInit();
	srvLstn();
}

void Server::srvInit()
{

	// register socket for server (a communication connection for TCP/IP Protocol)
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw(std::runtime_error("Error: socket"));

	_ServerAddress.sin_family = AF_INET;		 // set to IPv4
	_ServerAddress.sin_addr.s_addr = INADDR_ANY; // allows connection to any ip address
	_ServerAddress.sin_port = htons(_port);		 // converts from host byte to network byte

	int en = 1;
	// settings setsockopt to SO_REUSEADDR allows to use the same port directly after closing
	// with SOL_SOCKET we say that we want this action to our used _socket
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("Error: setsockopt"));

	// bind _socket to our described _ServerAddress
	if ((bind(_socket, (struct sockaddr *)&_ServerAddress, sizeof(_ServerAddress))) == -1)
		throw(std::runtime_error("Error: bind"));

	// set the _socket into a nonblocking modus.
	// Means that our _socket will directly return to start point if it is nothing receiving
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("Error: fcntl"));

	// marks our _socket to a passiv Socket which cann accept incoming connection.
	// also only allows _maxConnections connections at the same time
	if ((listen(_socket, _maxConnections)) == -1)
		throw(std::runtime_error("Error: listen"));
}

void Server::srvLstn()
{
	std::cout << blue << "~ Server is running | Port: " << _port << " | Password: " << _password << " ~ " << res << std::endl;
	std::cout << std::endl;
	fd_set read_set;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	while (Server::Signal == false)
	{
		// Reset read_set, re-set it to _socket and re-fill read_set with known _sockets
		FD_ZERO(&read_set);
		FD_SET(_socket, &read_set);
		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
			FD_SET(it->first, &read_set);
		// select waits for incoming connections and checks if we are receiving data to read
		int ret = select(_socket + 1, &read_set, NULL, NULL, &timeout);
		if (ret == -1 && Server::Signal == false)
			throw(std::runtime_error("Error: select"));
		else if (Server::Signal == false)
		{
			if (ret == 0)
			{
				// no activity
			}
			else
			{
				if (FD_ISSET(_socket, &read_set))
				{
					_ClientAddrLen = sizeof(_ClientAddress);
					int _clientSocket = accept(_socket, (struct sockaddr *)&_ClientAddress, &_ClientAddrLen);
					if (_clientSocket == -1)
						throw(std::runtime_error("Error: accept"));

					Client newClient;
					newClient.socket = _clientSocket;
					newClient.Nickname = "Nick" + Helper::itoa(_clientSocket);
					newClient.Username = "";
					newClient.UserMode = "";
					newClient.Hostname = "";
					newClient.Realname = "";
					newClient.Connected = true;
					clients[_clientSocket] = newClient;
					FD_SET(_clientSocket, &read_set);
				}
			}
			Recv();
			Check();
			Remv();
		}
	}
}

void Server::Remv()
{
	std::map<int, Client>::iterator it = clients.begin();

	while (it != clients.end())
	{
		if (!it->second.Connected || Server::Signal == true)
		{
			close(it->first);
			// return value <iterator> of erase() not used for linux compatibility
			clients.erase(it);
			it = clients.begin();
		}
		else
			++it;
	}
}

// receives incoming messages
void Server::Recv()
{
	std::map<int, Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (!it->second.Connected)
			continue;
		char buffer[1024];
		ssize_t bytes_received = recv(it->first, buffer, sizeof(buffer), 0);
		if (bytes_received < 1)
			continue;

		buffer[bytes_received] = '\0';
		it->second.recvMsg = std::string(buffer);
		std::cout << magenta << "srvRecv: " << it->second.recvMsg << res;
	}
}

// send messages
void Server::Check()
{
	std::map<int, Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (!it->second.Connected || it->second.recvMsg == "")
			continue;

		int socket = it->first;
		std::string msg = it->second.recvMsg;
		it->second.recvMsg = "";

		size_t pos = msg.find(" ");
		if (pos == std::string::npos)
			return;
		std::string command = msg.substr(0, pos);

		if (command == "CAP")
			return cmds->cap(socket, msg);
		if (command == "JOIN")
			return cmds->join(socket, msg);
		if (command == "PART")
			return cmds->part(socket, msg);
		if (command == "PRIVMSG")
			return cmds->privmsg(socket, msg);
		if (command == "NICK")
			return cmds->nick(socket, msg);
		if (command == "WHO")
			return cmds->who(socket, msg);
		if (command == "WHOIS")
			return cmds->whois(socket, msg);
		if (command == "MODE")
			return cmds->mode(socket, msg);
		if (command == "PING")
			return cmds->ping(socket, msg);
		if (command == "KICK")
			return cmds->kick(socket, msg);
		if (command == "QUIT")
			return cmds->quit(socket, msg);
		if (command == "TOPIC")
			return cmds->topic(socket, msg);
		// invite is a custom command of kvIRC and needs to be handled separatly
		std::transform(command.begin(), command.end(), command.begin(), ::toupper);
		if (command == "INVITE")
			return cmds->invite(socket, msg);
		cmds->unknown(socket, msg);
	}
}

// send messages
void Server::Send(int _clientSocket, const std::string &message)
{
	if (!send(_clientSocket, message.c_str(), message.size(), 0))
		std::cout << "Client: " << _clientSocket << " : ERROR" << std::endl;
	std::cout << green << "srvSend: " << message << res;
}
