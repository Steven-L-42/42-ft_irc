/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:38 by slippert          #+#    #+#             */
/*   Updated: 2024/03/27 15:58:15 by slippert         ###   ########.fr       */
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
	Server::Signal = true;
	std::cout << signum << std::endl;
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
			srvRecv();
			srvCheck();
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
			// return value <iterator> of erase() not used for linux compatibility
			clients.erase(it);
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
		std::cout << magenta << "srvRecv: " << it->second.recvMsg << res;
	}
}

// send messages
void Server::srvCheck()
{
	std::map<int, Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (!it->second.Connected)
			continue;
		checkCommand(it->first);
	}
}

// send messages
void Server::srvSend(int _clientSocket, const std::string &message)
{
	if (!send(_clientSocket, message.c_str(), message.size(), 0))
		std::cout << "Client: " << _clientSocket << " : ERROR" << std::endl;
	std::cout << green << "srvSend: " << message << res;
}

// check if incoming message is a command
void Server::checkCommand(int _clientSocket)
{
	std::string msg = clients[_clientSocket].recvMsg;
	if (msg == "")
		return;
	size_t pos = msg.find(" ");
	if (pos == std::string::npos)
		return;
	std::string command = msg.substr(0, pos);
	if (checkCAP(_clientSocket, msg, command))
		return;
	if (checkJOIN(_clientSocket, msg, command))
		return;
	if (checkPART(_clientSocket, msg, command))
		return;
	if (checkPRIVMSG(_clientSocket, msg, command))
		return;
	if (checkNICK(_clientSocket, msg, command))
		return;
	if (checkWHO(_clientSocket, msg, command))
		return;
	if (checkWHOIS(_clientSocket, msg, command))
		return;
	if (checkMODE(_clientSocket, msg, command))
		return;
	if (checkPING(_clientSocket, command))
		return;
	if (checkQUIT(_clientSocket, msg, command))
		return;
	if (checkTOPIC(_clientSocket, msg, command))
		return;
	// if a unknown command is entered
	if (unknownCMD(_clientSocket, msg))
		return;
}

// Join Channel function (command: /join <channel>)
bool Server::checkCAP(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("CAP" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		std::string replyMsg;

		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		if (*itToken == "LS")
		{
			replyMsg = "CAP * LS :multi-prefix sasl\r\n";
			srvSend(_clientSocket, replyMsg);
			return (true);
		}
		if (*itToken == "REQ")
		{
			// substract all after "CAP REQ "
			itToken++;
			replyMsg = "CAP * ACK " + *itToken + CRLF;
			srvSend(_clientSocket, replyMsg);
			return (true);
		}
		if (*itToken == "END")
		{
			std::string incomingMsg;
			// wait for receiving NICK <username and USER <username> <usermode> <hostname> <clientcomment>
			size_t breakit = 0;
			while (Server::Signal == false)
			{
				char buffer[1024];
				ssize_t bytes_received = recv(_clientSocket, buffer, sizeof(buffer), 0);
				if (bytes_received < 1)
				{
					// uncool solution ^^
					if (++breakit >= 99999999)
						break;
					continue;
				}

				buffer[bytes_received] = '\0';
				incomingMsg = std::string(buffer);
				break;
			}
			std::cout << magenta << "srvRecv: " << incomingMsg << res;

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
				srvSend(_clientSocket, replyMsg);
				clients[_clientSocket].Connected = false;
				return (true);
			}

			// set <nickname> and send reply to client
			if (itToken != strTokens.end() && *itToken == "NICK")
			{
				if (++itToken != strTokens.end())
					checkNICK(_clientSocket, "NICK " + *itToken + "\r\n", "NICK");
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

			// send welcome message and login
			replyMsg = RPL_WELCOME(clients[_clientSocket].Nickname);
			srvSend(_clientSocket, replyMsg);
			// send message of the day
			replyMsg = RPL_MOTD();
			srvSend(_clientSocket, replyMsg);
		}
	}
	return ("CAP" == cmd);
}

// Join Channel function (command: /join <channel>)
bool Server::checkJOIN(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("JOIN" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		bool isNewChannel = false;
		std::string replyMsg;

		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string tok_Channel = *itToken;
		std::string tok_Passsword = "";
		if (++itToken != strTokens.end())
			tok_Passsword = *itToken;

		if (channels.find(tok_Channel) == channels.end())
		{
			Channel newChannel;
			newChannel.Password = tok_Passsword;
			newChannel.Modes = "";
			newChannel.Topic = "No Topic";
			channels[tok_Channel] = newChannel;
			isNewChannel = true;
		}
		else
		{
			if (channels[tok_Channel].Password != tok_Passsword)
			{
				replyMsg = ERR_PASSWDMISMATCH(clients[_clientSocket].Nickname);
				srvSend(_clientSocket, replyMsg);
				return (true);
			}
		}

		// set REPLY MSG and send it back to clientSocket
		replyMsg = RPL_JOINMSG(clients[_clientSocket].Nickname, clients[_clientSocket].Hostname, tok_Channel);

		// add channel to user
		clients[_clientSocket].channel[tok_Channel].isJoined = true;

		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.channel[tok_Channel].isJoined == true)
				srvSend(it->first, replyMsg);
		}

		if (isNewChannel)
		{
			clients[_clientSocket].channel[tok_Channel].isOp = true;
			replyMsg = RPL_UMODEIS(clients[_clientSocket].Hostname, tok_Channel, "+o", clients[_clientSocket].Nickname);
			srvSend(_clientSocket, replyMsg);
		}
	}
	return ("JOIN" == cmd);
}

// Leave Channel function (command: /leave)
bool Server::checkPART(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("PART" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string ChannelName = *itToken;
		// set REPLY MSG and send it back to clientSocket
		std::string replyMsg = RPL_PART(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, ChannelName);
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.channel[ChannelName].isJoined == true)
				srvSend(it->first, replyMsg);
		}
		// remove channel from user
		if (clients[_clientSocket].channel[ChannelName].isJoined == true)
			clients[_clientSocket].channel.erase(clients[_clientSocket].channel.find(ChannelName));
	}
	return ("PART" == cmd);
}

// Private & Channel Message function (command: <msg>)
bool Server::checkPRIVMSG(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("PRIVMSG" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string NickOrChannel = *itToken;
		std::string message = "";
		itToken++;
		while (itToken != strTokens.end())
		{
			message += *itToken;
			if (++itToken != strTokens.end())
				message += " ";
		}
		std::string replyMsg = RPL_MESSAGE(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, NickOrChannel, message);
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->first == _clientSocket && clients[_clientSocket].Nickname != NickOrChannel)
				continue;
			srvSend(it->first, replyMsg);
		}
	}
	return ("PRIVMSG" == cmd);
}

// Change nickname function (command: /nick <nickname>)
bool Server::checkNICK(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("NICK" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string clientNickname = *itToken;
		// set REPLY MSG and send it back to clientSocket
		std::string replyMsg = RPL_NICKCHANGE(clients[_clientSocket].Nickname, clientNickname);
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
			srvSend(it->first, replyMsg);
		// overwrite old nickname
		clients[_clientSocket].Nickname = clientNickname;
	}
	return ("NICK" == cmd);
}

// WHOIS gives information about a specific user with /whois <nickname>
bool Server::checkWHOIS(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("WHOIS" == cmd)
	{
		std::string replyMsg;
		clients[_clientSocket].recvMsg = "";
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string clientNickname = *itToken;
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.Nickname == clientNickname)
				break;
		}
		if (it == clients.end())
			return (true);

		replyMsg = RPL_WHOIS(clients[_clientSocket].Nickname, clientNickname, it->second.Username, it->second.Hostname, it->second.Realname);
		srvSend(_clientSocket, replyMsg);
	}
	return ("WHOIS" == cmd);
}

// WHO sends a list with all nicknames on same channel
bool Server::checkWHO(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("WHO" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		std::string replyMsg;

		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string channelName = *itToken;

		// send Channel Topic
		replyMsg = RPL_CHTOPIC(clients[_clientSocket].Nickname, channelName, channels[channelName].Topic);
		srvSend(_clientSocket, replyMsg);
		// create userlist based on stored information of clients
		std::string userList = "";
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end();)
		{
			if (it->second.channel[channelName].isJoined == true)
			{
				userList += (it->second.channel[channelName].isOp ? "@" : "") + it->second.Nickname;
				it++;
				if (it != clients.end())
					userList += " ";
			}
			else
				it++;
		}

		// send all channel user nicknames to client
		replyMsg = RPL_USERLIST(clients[_clientSocket].Nickname, channelName, userList);
		srvSend(_clientSocket, replyMsg);
		// send a end of userlist for client to know that this list is ending
		replyMsg = RPL_ENDOFUSERLIST(clients[_clientSocket].Nickname, channelName);
		srvSend(_clientSocket, replyMsg);
	}
	return ("WHO" == cmd);
}

// Send Channel modes on request
bool Server::checkMODE(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("MODE" == cmd)
	{
		(void)msg;
		(void)cmd;
		// clients[_clientSocket].recvMsg = "";
		// // substract channel name without # and without \r\n
		// std::string substrCHANNEL = msg.substr(modeSize, msg.size() - modeSize - 2);
		// std::string replyMsg;

		// // send Channel modes
		// replyMsg = RPL_CHANNELMODEIS(clients[_clientSocket].Nickname, substrCHANNEL, " ");
		// srvSend(_clientSocket, replyMsg);

		clients[_clientSocket].recvMsg = "";
		std::string replyMsg = ": 421 " + clients[_clientSocket].Nickname + " MODE :Command is currently deactivated by Steven!" + CRLF;
		srvSend(_clientSocket, replyMsg);
		return (true);
	}
	return ("MODE" == cmd);
}

// PING response with PONG
bool Server::checkPING(int _clientSocket, const std::string &cmd)
{
	if ("PING" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		std::string replyMsg = RPL_PONG(clients[_clientSocket].Nickname, clients[_clientSocket].Hostname);
		srvSend(_clientSocket, replyMsg);
	}
	return ("PING" == cmd);
}

// QUIT Server function with message
bool Server::checkQUIT(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("QUIT" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		// substract whole message after 'QUIT :'
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string quitMessage = *itToken;

		// prepare reply and set connected to false
		std::string replyMsg = RPL_QUIT(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, quitMessage);
		clients[_clientSocket].Connected = false;
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
			srvSend(it->first, replyMsg);
	}
	return ("QUIT" == cmd);
}

// TOPIC set new Topic (/topic <channel> <msg>)
bool Server::checkTOPIC(int _clientSocket, const std::string &msg, const std::string &cmd)
{
	if ("TOPIC" == cmd)
	{
		clients[_clientSocket].recvMsg = "";
		std::string replyMsg;

		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string channelName = *itToken;

		// Do nothing if # ist not in front of channelname (compared to official Server)
		if (channelName[0] != '#')
			return (true);

		// Send current channel topic to client
		if (strTokens.size() == 2)
		{
			// send Channel Topic
			replyMsg = RPL_CHTOPIC(clients[_clientSocket].Nickname, channelName, channels[channelName].Topic);
			srvSend(_clientSocket, replyMsg);
			return (true);
		}
		// if more then 2 && client is Operator then change channel topic
		if (clients[_clientSocket].channel[channelName].isOp == true)
		{
			itToken++;
			std::string message = "";
			while (itToken != strTokens.end())
			{
				message += *itToken;
				if (++itToken != strTokens.end())
					message += " ";
			}
			message.erase(0, 1);
			channels[channelName].Topic = message;
			replyMsg = RPL_SETCHTOPIC(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, channelName, message);
			srvSend(_clientSocket, replyMsg);
		}
		else
		{
			// client is not an operator - Error
			replyMsg = ERR_CHANOPRIVSNEEDED(clients[_clientSocket].Nickname, channelName);
			srvSend(_clientSocket, replyMsg);
		}
	}
	return ("TOPIC" == cmd);
}

// UNKNOWN Command send Error code to User
bool Server::unknownCMD(int _clientSocket, const std::string &msg)
{
	clients[_clientSocket].recvMsg = "";
	std::string replyMsg = ERR_UNKNOWNCOMMAND(clients[_clientSocket].Nickname, msg);
	srvSend(_clientSocket, replyMsg);
	return (true);
}