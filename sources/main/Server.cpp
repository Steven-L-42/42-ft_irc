#include "../../includes/Server.hpp"

bool Server::Signal = false;

Server::Server(int port, std::string password) : cmds(NULL), _port(port), _password(password)
{
	cmds = new Commands(this);
}

Server::~Server()
{
	Server::Signal = true;
	Remv();
	close(_socket);
	delete (cmds);
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
	return (Signal);
}

void Server::SignalHandler(int signum)
{
	(void)signum;
	Server::Signal = true;
	std::cout << red << "kill signal received!" << res << std::endl;
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

	int en = 1; // activates the option SO_REUSEADDR

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

	// marks our _socket to a passive socket which can accept incoming connection.
	// also only allows _maxConnections connections at the same time
	if ((listen(_socket, _maxConnections)) == -1)
		throw(std::runtime_error("Error: listen"));
}

void Server::srvLstn()
{
	std::cout << blue << "\n~ Server is running | Port: " << _port << " | Password: " << _password << " ~ \n"
			  << res << std::endl;

	fd_set read_set;

	// create timeval struct and set timeout to 0 (non blocking).
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	while (Server::Signal == false)
	{
		// resetting read_set each cycle ensures that only the current active sockets will be used when select is called.
		FD_ZERO(&read_set);
		// add main socket to read_set.
		FD_SET(_socket, &read_set);
		// add all active clientSockets to read_set.
		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
			FD_SET(it->first, &read_set);
		// select waits for incoming connections and checks if we are receiving data to read
		// also, select now listens to the addet connections in read_set.
		// means: if we reach the limit of _maxConnections, select will still listen to the known stored clientSockets.
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
				// check if main socket is available in read_set
				if (FD_ISSET(_socket, &read_set))
				{
					_ClientAddrLen = sizeof(_ClientAddress);
					int _clientSocket = accept(_socket, (struct sockaddr *)&_ClientAddress, &_ClientAddrLen);
					if (_clientSocket == -1)
						throw(std::runtime_error("Error: accept"));

					// convert address to ip address and network byte port to int port
					unsigned long clientIP = ntohl(_ClientAddress.sin_addr.s_addr);
					int clientPort = ntohs(_ClientAddress.sin_port);
					// format and output ip address in common known format
					std::string formattedIP = Helper::itoa((clientIP >> 24) & 0xFF) + "." + Helper::itoa((clientIP >> 16) & 0xFF) + "." + Helper::itoa((clientIP >> 8) & 0xFF) + "." + Helper::itoa((clientIP) & 0xFF);
					std::cout << blue << "New connection from IP: " << formattedIP << ", Port: " << clientPort << res << std::endl;

					Client newClient;
					newClient.socket = _clientSocket;
					newClient.Nickname = "Nick" + Helper::itoa(_clientSocket);
					newClient.Username = "";
					newClient.UserMode = "";
					newClient.Hostname = "";
					newClient.Realname = "";
					newClient.Password = "";
					newClient.Connected = true;
					newClient.Accepted = false;
					newClient.Registred = false;
					newClient.LoginProcess = "NEW";
					clients[_clientSocket] = newClient;
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

	std::map<std::string, Channel>::iterator channel_it = channels.begin();

	while (channel_it != channels.end())
	{
		if (channel_it->second.user_count == 0)
		{
			channels.erase(channel_it);
			channel_it = channels.begin();
		}
		else
			++channel_it;
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
		if (bytes_received <= 0)
		{
			if (bytes_received == 0)
			{
				// std::cout << "Client: " << it->first << " : Error RECV" << std::endl;
				cmds->quit(it->first, "quit .Connection lost");
			}
			continue;
		}

		buffer[bytes_received] = '\0';
		it->second.recvMsg += std::string(buffer);
		if (it->second.recvMsg[it->second.recvMsg.size() - 1] == '\n')
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
		if (it->second.recvMsg[it->second.recvMsg.size() - 1] != '\n')
			continue;
		std::string msg = it->second.recvMsg;
		it->second.recvMsg = "";

		size_t pos = msg.find(" ");
		if (pos == std::string::npos)
			return;
		std::string command = msg.substr(0, pos);

		if (command == "CAP" || it->second.LoginProcess == "END")
			return cmds->cap(socket, msg);
		if (command == "QUIT")
			return cmds->quit(socket, msg);
		if (it->second.Registred == false)
			return;
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
			return cmds->ping(socket);
		if (command == "KICK")
			return cmds->kick(socket, msg);
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
		std::cout << "Client: " << _clientSocket << " : Error SEND" << std::endl;
	std::cout << green << "srvSend: " << message << res;
}
