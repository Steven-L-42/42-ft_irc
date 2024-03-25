/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:38 by slippert          #+#    #+#             */
/*   Updated: 2024/03/24 15:57:59 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool Server::Signal = false;

Server::Server(int port, std::string password) : _port(port), _password(password)
{
}

Server::~Server()
{
	Server::Signal = true;
	srvRemv();
	close(_socket);
}

void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << "killed Signal" << std::endl;
	Server::Signal = true;
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

std::string _serverName = "MyIRCServer";

void Server::srvLstn()
{
	std::cout << blue << "~ Server is running | Port: " << _port << " ~ " << res << std::endl;
	std::cout << std::endl;
	fd_set read_set;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	std::cout << "Server Socket: " << _socket << std::endl;
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
			srvRecv();
			srvSend();
			srvRemv();
		}
	}
}

void Server::srvRemv()
{
	std::map<int, Client>::iterator it = clients.begin();

	while (it != clients.end())
	{
		if (!it->second.Connected || Server::Signal == true)
		{
			close(it->first);
			clients.erase(it); // Direktes Löschen ohne Zuweisung an 'it'
			it = clients.begin();
		}
		else
			++it;
	}
}

// receives incoming messages
void Server::srvRecv()
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
		std::cout << "srvRecv: " << it->second.recvMsg;
	}
}

// send messages
void Server::srvSend()
{
	std::map<int, Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (!it->second.Connected)
			continue;
		checkCommand(it->first);
	}
}

// check if incoming message is a command
void Server::checkCommand(int _clientSocket)
{
	std::string msg = clients[_clientSocket].recvMsg;
	if (msg == "")
		return;

	if (checkCAP(_clientSocket, msg))
		return;
	if (checkJOIN(_clientSocket, msg))
		return;
	if (checkPART(_clientSocket, msg))
		return;
	if (checkPRIVMSG(_clientSocket, msg))
		return;
	if (checkNICK(_clientSocket, msg))
		return;
	if (checkWHO(_clientSocket, msg))
		return;
	if (checkWHOIS(_clientSocket, msg))
		return;
	if (checkMODE(_clientSocket, msg))
		return;
	if (checkPING(_clientSocket, msg))
		return;
	if (checkQUIT(_clientSocket, msg))
		return;
	// if a unknown Command is entered
	if (unknownCMD(_clientSocket, msg))
		return;
}

// Join Channel function (command: /join <channel>)
bool Server::checkCAP(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 5)
		return false;
	std::string cap = "CAP ";
	int capSize = cap.size();

	std::string substrCAP = msg.substr(0, capSize);

	if (cap == substrCAP)
	{
		clients[_clientSocket].recvMsg = "";
		std::string replyMsg = "";
		std::string substrCMD;
		substrCMD = msg.substr(capSize, 2);
		if (substrCMD == "LS")
		{
			replyMsg = "CAP * LS :multi-prefix sasl\r\n";
			if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << _clientSocket << ": ERROR" << std::endl;
			// std::cout << substrCMD << std::endl;
			return (cap == substrCAP);
		}
		substrCMD = msg.substr(capSize, 3);
		if (substrCMD == "REQ")
		{
			// substract all after "CAP REQ :"
			substrCMD = msg.substr(capSize + substrCMD.size() + 2, msg.size() - (capSize + substrCMD.size() + 4));
			// std::cout << substrCMD << std::endl;
			replyMsg = "CAP * ACK :" + substrCMD + CRLF;
			// std::cout << replyMsg;
			if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << _clientSocket << ": ERROR" << std::endl;
			return (cap == substrCAP);
		}
		substrCMD = msg.substr(capSize, 3);
		if (substrCMD == "END")
		{
			std::string incomingMsg;
			// wait for receiving NICK <username and USER <username> <usermode> <hostname> <clientcomment>
			while (Server::Signal == false)
			{
				char buffer[1024];
				ssize_t bytes_received = recv(_clientSocket, buffer, sizeof(buffer), 0);
				if (bytes_received < 1)
					continue;

				buffer[bytes_received] = '\0';
				incomingMsg = std::string(buffer);
				break;
			}
			// std::cout << incomingMsg;

			// create tokens, split string on each space ' '.
			std::vector<std::string> strTokens = Helper::splitString(incomingMsg);
			std::vector<std::string>::iterator itToken = strTokens.begin();

			bool foundPASS = false;
			// set <password>
			if (itToken != strTokens.end() && *itToken == "PASS")
			{
				if (++itToken != strTokens.end() && !itToken->empty())
					itToken->erase(0, 1);
				if (*itToken == _password)
				{
					foundPASS = true;
					clients[_clientSocket].Password = *itToken;
				}
				++itToken;
			}
			if (!foundPASS)
			{
				replyMsg = ERR_PASSWDMISMATCH(clients[_clientSocket].Nickname);
				if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
					std::cout << _clientSocket << ": ERROR" << std::endl;
				clients[_clientSocket].Connected = false;
				return (cap == substrCAP);
			}

			// std::cout << "TEST" << msg << std::endl;
			// set <nickname> and send reply to client
			if (itToken != strTokens.end() && *itToken == "NICK")
			{
				if (++itToken != strTokens.end())
					checkNICK(_clientSocket, "NICK " + *itToken + "\r\n");
				++itToken;
			}
			if (itToken != strTokens.end() && *itToken == "USER")
			{
				// set <username>
				if (++itToken != strTokens.end())
					clients[_clientSocket].Username = *itToken;
				// set <usermode>
				if (++itToken != strTokens.end())
					clients[_clientSocket].UserMode = *itToken;
				// set <hostname>
				if (++itToken != strTokens.end())
					clients[_clientSocket].Hostname = *itToken;
				// set <realname>
				// skip ':1,8' (kvIRC comment)
				if (++itToken != strTokens.end() && !itToken->empty())
					itToken->erase(0, 5);
				while (itToken != strTokens.end())
				{
					clients[_clientSocket].Realname += *itToken;
					if (++itToken != strTokens.end())
						clients[_clientSocket].Realname += " ";
				}
			}

			// send Welcome Message and login
			replyMsg = RPL_WELCOME(clients[_clientSocket].Nickname);
			if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << _clientSocket << ": ERROR" << std::endl;
			// std::cout << replyMsg;

			// send Message of the Day
			replyMsg = RPL_MOTD();
			if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << _clientSocket << ": ERROR" << std::endl;
			// std::cout << replyMsg;
			return (cap == substrCAP);
		}
	}
	return (cap == substrCAP);
}

// Join Channel function (command: /join <channel>)
bool Server::checkJOIN(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 7)
		return false;
	std::string join = "JOIN #";
	int joinSize = join.size();

	std::string substrCMD = msg.substr(0, joinSize);

	if (join == substrCMD)
	{
		clients[_clientSocket].recvMsg = "";
		std::string substrCHANNEL = msg.substr(joinSize, msg.size() - joinSize);
		// set REPLY MSG and send it back to clientSocket
		std::string replyMsg = RPL_JOINMSG(clients[_clientSocket].Nickname, clients[_clientSocket].Hostname, substrCHANNEL);
		// delete received Message
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			// if (it->first == _clientSocket)
			// 	continue;
			if (!send(it->first, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << it->first << ": ERROR" << std::endl;
		}
		substrCHANNEL = substrCHANNEL.substr(0, substrCHANNEL.size() - 2);
		if (std::find(clients[_clientSocket].channel.begin(), clients[_clientSocket].channel.end(), substrCHANNEL) == clients[_clientSocket].channel.end())
			clients[_clientSocket].channel.push_back(substrCHANNEL);
	}
	return (join == substrCMD);
}

// Leave Channel function (command: /leave)
bool Server::checkPART(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 6)
		return false;
	std::string part = "PART ";
	int partSize = part.size();

	std::string substrCMD = msg.substr(0, partSize);

	if (part == substrCMD)
	{
		std::string substrCHANNEL = msg.substr(partSize, msg.size() - partSize);
		// set REPLY MSG and send it back to clientSocket
		std::string replyMsg = RPL_PART(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, substrCHANNEL);

		// delete received Message
		clients[_clientSocket].recvMsg = "";
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (!send(it->first, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << it->first << ": ERROR" << std::endl;
		}
		substrCHANNEL = substrCHANNEL.substr(0, substrCHANNEL.size() - 2);
		if (std::find(clients[_clientSocket].channel.begin(), clients[_clientSocket].channel.end(), substrCHANNEL) != clients[_clientSocket].channel.end())
			clients[_clientSocket].channel.erase(std::find(clients[_clientSocket].channel.begin(), clients[_clientSocket].channel.end(), substrCHANNEL));
	}
	return (part == substrCMD);
}

// Private & Channel Message function (command: <msg>)
bool Server::checkPRIVMSG(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 9)
		return false;
	std::string privmsg = "PRIVMSG ";
	int privmsgSize = privmsg.size();
	std::string substrCMD = msg.substr(0, privmsgSize);

	if (privmsg == substrCMD)
	{
		clients[_clientSocket].recvMsg = "";
		int max;
		int start;
		int i = privmsgSize;

		// search : position
		while (msg[i] && msg[i] != ':')
			i++;
		// set max number to subtract
		max = i - privmsgSize;
		std::string substrChannelOrName = msg.substr(privmsgSize, max - 1);
		// set new start to subtract
		start = privmsgSize + max + 1;
		// set new max to subtract - 2 (CLRF)
		max = msg.size() - start - 2;
		std::string substrMessage = msg.substr(start, max);
		// set REPLY MSG and send it back to clientSocket
		std::string replyMsg = RPL_MESSAGE(clients[_clientSocket].Nickname, clients[_clientSocket].Hostname, substrChannelOrName, substrMessage);
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->first == _clientSocket && clients[_clientSocket].Nickname != substrChannelOrName)
				continue;
			if (!send(it->first, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << it->first << ": ERROR" << std::endl;
		}
	}
	return (privmsg == substrCMD);
}

// Change nickname function (command: /nick <nickname>)
bool Server::checkNICK(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 6)
		return false;
	std::string nickmsg = "NICK ";
	int nickSize = nickmsg.size();
	std::string substrCMD = msg.substr(0, nickSize);

	if (nickmsg == substrCMD)
	{
		clients[_clientSocket].recvMsg = "";
		// substract new nickname from received msg
		std::string substrNICKNAME = msg.substr(nickSize, msg.size() - nickSize - 2);

		// set REPLY MSG and send it back to clientSocket
		std::string replyMsg = RPL_NICKCHANGE(clients[_clientSocket].Nickname, substrNICKNAME);
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			// if (it->first == _clientSocket)
			// 	continue;
			if (!send(it->first, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << it->first << ": ERROR" << std::endl;
		}
		// overwrite old nickname
		clients[_clientSocket].Nickname = substrNICKNAME;
	}
	return (nickmsg == substrCMD);
}

// WHOIS gives information about a specific user with /whois <nickname>
bool Server::checkWHOIS(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 7)
		return false;
	std::string whomsg = "WHOIS ";
	int whoisSize = whomsg.size();
	std::string substrCMD = msg.substr(0, whoisSize);

	if (whomsg == substrCMD)
	{
		clients[_clientSocket].recvMsg = "";
		std::string substrNAME = msg.substr(whoisSize, msg.size() - whoisSize - 2);
		// std::cout << substrNAME << "$" << std::endl;
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.Nickname == substrNAME)
				break;
		}
		if (it == clients.end())
			return (whomsg == substrCMD);

		std::string replyMsg = RPL_WHOIS(clients[_clientSocket].Nickname, substrNAME, it->second.Username, it->second.Hostname, it->second.Realname);
		if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
			std::cout << _clientSocket << ": ERROR" << std::endl;
		// std::cout << replyMsg << std::endl;
	}
	return (whomsg == substrCMD);
}

// WHO sends a list with all nicknames on same channel
bool Server::checkWHO(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 5)
		return false;

	std::string whomsg = "WHO ";
	int whoSize = whomsg.size();
	std::string substrCMD = msg.substr(0, whoSize);

	if (whomsg == substrCMD)
	{
		clients[_clientSocket].recvMsg = "";
		// substract channel name without # and without \r\n
		std::string substrCHANNEL = msg.substr(whoSize + 1, msg.size() - whoSize - 3);

		std::string replyMsg;

		// send Channel Topic
		replyMsg = RPL_CHTOPIC(clients[_clientSocket].Nickname, substrCHANNEL, "Mein Topic");
		if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
			std::cout << _clientSocket << ": ERROR" << std::endl;

		// create userlist based on stored information of clients
		std::string userList = "";
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end();)
		{
			if (std::find(it->second.channel.begin(), it->second.channel.end(), substrCHANNEL) != it->second.channel.end())
			{
				userList += it->second.Nickname;
				it++;
				if (it != clients.end())
					userList += " ";
			}
			else
				it++;
		}

		// send all channel user nicknames to client
		replyMsg = RPL_USERLIST(clients[_clientSocket].Nickname, substrCHANNEL, userList);
		if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
			std::cout << _clientSocket << ": ERROR" << std::endl;

		// send a end of userlist for client to know that this list is ending
		replyMsg = RPL_ENDOFUSERLIST(clients[_clientSocket].Nickname, substrCHANNEL);
		if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
			std::cout << _clientSocket << ": ERROR" << std::endl;
	}
	return (whomsg == substrCMD);
}

// Send Channel modes on request
bool Server::checkMODE(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 7)
		return false;

	std::string mode = "MODE #";
	int modeSize = mode.size();
	std::string substrCMD = msg.substr(0, modeSize);

	if (mode == substrCMD)
	{
		clients[_clientSocket].recvMsg = "";
		// substract channel name without # and without \r\n
		std::string substrCHANNEL = msg.substr(modeSize, msg.size() - modeSize - 2);
		std::string replyMsg;

		// send Channel modes
		replyMsg = RPL_CHANNELMODEIS(clients[_clientSocket].Nickname, substrCHANNEL, "m");
		if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
			std::cout << _clientSocket << ": ERROR" << std::endl;
	}
	return (mode == substrCMD);
}

// PING response with PONG
bool Server::checkPING(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 6)
		return false;

	std::string ping = "PING ";
	int modeSize = ping.size();
	std::string substrCMD = msg.substr(0, modeSize);

	if (ping == substrCMD)
	{
		clients[_clientSocket].recvMsg = "";
		std::string replyMsg = RPL_PONG(clients[_clientSocket].Nickname, clients[_clientSocket].Hostname);
		if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
			std::cout << _clientSocket << ": ERROR" << std::endl;
	}
	return (ping == substrCMD);
}

// QUIT Server function with message
bool Server::checkQUIT(int _clientSocket, const std::string &msg)
{
	if (msg.size() < 6)
		return false;

	std::string quit = "QUIT ";
	int quitSize = quit.size();
	std::string substrCMD = msg.substr(0, quitSize);
	if (quit == substrCMD)
	{
		clients[_clientSocket].recvMsg = "";
		// substract whole message after 'QUIT :'
		std::string substrMESSAGE = msg.substr(quitSize + 1, msg.size() - (quitSize + 3));

		// prepare reply and set connected to false
		std::string replyMsg = RPL_QUIT(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, substrMESSAGE);
		clients[_clientSocket].Connected = false;
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (!send(it->first, replyMsg.c_str(), replyMsg.size(), 0))
				std::cout << it->first << ": ERROR" << std::endl;
		}
	}
	return (quit == substrCMD);
}

// UNKNOWN Command send Error code to User
bool Server::unknownCMD(int _clientSocket, const std::string &msg)
{
	clients[_clientSocket].recvMsg = "";
	std::string replyMsg = ERR_UNKNOWNCOMMAND(clients[_clientSocket].Nickname, msg);
	if (!send(_clientSocket, replyMsg.c_str(), replyMsg.size(), 0))
		std::cout << _clientSocket << ": ERROR" << std::endl;
	return (true);
}