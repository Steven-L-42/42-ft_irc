#pragma once

#include <iostream>
#include "../Helper.hpp"
#include "../Server.hpp"
#include "../Client.hpp"
#include <map>

//
class COMMANDS
{
private:
	Server *srv;
	std::map<int, Client> *clients;

public:
	COMMANDS(Server *_srv);
	~COMMANDS();
	// void cap(Server *srv, int socket, const std::string &msg);
	// void nick(Server *srv, int socket, const std::string &msg);
	// void join(Server *srv, int socket, const std::string &msg);
	// void kick(Server *srv, int socket, const std::string &msg);
	// void mode(Server *srv, int socket, const std::string &msg);
	// void part(Server *srv, int socket, const std::string &msg);
	// void ping(Server *srv, int socket, const std::string &msg);
	// void privmsg(Server *srv, int socket, const std::string &msg);
	// void quit(Server *srv, int socket, const std::string &msg);
	// void topic(Server *srv, int socket, const std::string &msg);
	// void unknown(Server *srv, int socket, const std::string &msg);
	// void who(Server *srv, int socket, const std::string &msg);
	void whois(Server *srv, int socket, const std::string &msg);
};

COMMANDS::COMMANDS(Server *_srv) : srv(_srv), clients(srv->getClients())
{
}
COMMANDS::~COMMANDS()
{
}

// void COMMANDS::whois(Server *srv, int socket, const std::string &msg)
// {

// 	std::string replyMsg;
// 	clients-
// 	clients[socket].recvMsg = "";
// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	std::string clientNickname = *itToken;
// 	std::map<int, Client>::iterator it;
// 	for (it = clients.begin(); it != clients.end(); it++)
// 	{
// 		if (it->second.Nickname == clientNickname)
// 			break;
// 	}
// 	if (it == srv->clients.end())
// 		return;

// 	replyMsg = RPL_WHOIS(clients[socket].Nickname, clientNickname, it->second.Username, it->second.Hostname, it->second.Realname);
// 	srv->Send(socket, replyMsg);
// }

// void COMMANDS::who(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	std::string replyMsg;

// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	std::string channelName = *itToken;

// 	// send Channel Topic
// 	replyMsg = RPL_CHTOPIC(srv->clients[socket].Nickname, channelName, channels[channelName].Topic);
// 	srv->Send(socket, replyMsg);
// 	// create userlist based on stored information of clients
// 	std::string userList = "";
// 	std::map<int, Client>::iterator it;
// 	for (it = srv->clients.begin(); it != srv->clients.end();)
// 	{
// 		if (it->second.channel[channelName].isJoined == true)
// 		{
// 			userList += (it->second.channel[channelName].isOp ? "@" : "") + it->second.Nickname;
// 			it++;
// 			if (it != srv->clients.end())
// 				userList += " ";
// 		}
// 		else
// 			it++;
// 	}

// 	// send all channel user nicknames to client
// 	replyMsg = RPL_USERLIST(srv->clients[socket].Nickname, channelName, userList);
// 	srv->Send(socket, replyMsg);
// 	// send a end of userlist for client to know that this list is ending
// 	replyMsg = RPL_ENDOFUSERLIST(srv->clients[socket].Nickname, channelName);
// 	srv->Send(socket, replyMsg);
// }

// void COMMANDS::unknown(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	std::string replyMsg = ERR_UNKNOWNCOMMAND(srv->clients[socket].Nickname, msg);
// 	srv->Send(socket, replyMsg);
// }

// void COMMANDS::topic(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	std::string replyMsg;

// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	std::string channelName = *itToken;

// 	// Do nothing if # ist not in front of channelname (compared to official Server)
// 	if (channelName[0] != '#')
// 		return;

// 	// Send current channel topic to client
// 	if (strTokens.size() == 2)
// 	{
// 		// send Channel Topic
// 		replyMsg = RPL_CHTOPIC(srv->clients[socket].Nickname, channelName, channels[channelName].Topic);
// 		srv->Send(socket, replyMsg);
// 		return;
// 	}
// 	// if more then 2 && client is Operator then change channel topic
// 	if (srv->clients[socket].channel[channelName].isOp == true)
// 	{
// 		itToken++;
// 		std::string message = "";
// 		while (itToken != strTokens.end())
// 		{
// 			message += *itToken;
// 			if (++itToken != strTokens.end())
// 				message += " ";
// 		}
// 		message.erase(0, 1);
// 		channels[channelName].Topic = message;
// 		replyMsg = RPL_SETCHTOPIC(srv->clients[socket].Nickname, srv->clients[socket].Username, srv->clients[socket].Hostname, channelName, message);
// 		srv->Send(socket, replyMsg);
// 	}
// 	else
// 	{
// 		// client is not an operator - Error
// 		replyMsg = ERR_CHANOPRIVSNEEDED(srv->clients[socket].Nickname, channelName);
// 		srv->Send(socket, replyMsg);
// 	}
// }

// void COMMANDS::quit(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	// substract whole message after 'QUIT :'
// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	std::string quitMessage = *itToken;

// 	// prepare reply and set connected to false
// 	std::string replyMsg = RPL_QUIT(srv->clients[socket].Nickname, srv->clients[socket].Username, srv->clients[socket].Hostname, quitMessage);
// 	srv->clients[socket].Connected = false;
// 	std::map<int, Client>::iterator it;
// 	for (it = srv->clients.begin(); it != srv->clients.end(); it++)
// 		srv->Send(it->first, replyMsg);
// }

// void COMMANDS::privmsg(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	std::string NickOrChannel = *itToken;
// 	std::string message = "";
// 	itToken++;
// 	while (itToken != strTokens.end())
// 	{
// 		message += *itToken;
// 		if (++itToken != strTokens.end())
// 			message += " ";
// 	}
// 	std::string replyMsg = RPL_MESSAGE(srv->clients[socket].Nickname, srv->clients[socket].Username, srv->clients[socket].Hostname, NickOrChannel, message);
// 	std::map<int, Client>::iterator it;
// 	for (it = srv->clients.begin(); it != srv->clients.end(); it++)
// 	{
// 		if (it->first == socket && srv->clients[socket].Nickname != NickOrChannel)
// 			continue;
// 		srv->Send(it->first, replyMsg);
// 	}
// }
// void COMMANDS::ping(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	std::string replyMsg = RPL_PONG(srv->clients[socket].Nickname, srv->clients[socket].Hostname);
// 	srv->Send(socket, replyMsg);
// }

// void COMMANDS::part(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	std::string ChannelName = *itToken;
// 	// set REPLY MSG and send it back to clientSocket
// 	std::string replyMsg = RPL_PART(srv->clients[socket].Nickname, srv->clients[socket].Username, srv->clients[socket].Hostname, ChannelName);
// 	std::map<int, Client>::iterator it;
// 	for (it = srv->clients.begin(); it != srv->clients.end(); it++)
// 	{
// 		if (it->second.channel[ChannelName].isJoined == true)
// 			srv->Send(it->first, replyMsg);
// 	}
// 	// remove channel from user
// 	if (srv->clients[socket].channel[ChannelName].isJoined == true)
// 		srv->clients[socket].channel.erase(srv->clients[socket].channel.find(ChannelName));
// }

// void COMMANDS::mode(Server *srv, int socket, const std::string &msg)
// {
// 	(void)msg;
// 	(void)cmd;
// 	// clients[socket].recvMsg = "";
// 	// // substract channel name without # and without \r\n
// 	// std::string substrCHANNEL = msg.substr(modeSize, msg.size() - modeSize - 2);
// 	// std::string replyMsg;

// 	// // send Channel modes
// 	// replyMsg = RPL_CHANNELMODEIS(srv->clients[socket].Nickname, substrCHANNEL, " ");
// 	// srv->Send(socket, replyMsg);

// 	srv->clients[socket].recvMsg = "";
// 	std::string replyMsg = ": 421 " + srv->clients[socket].Nickname + " MODE :Command is currently deactivated by Steven!" + CRLF;
// 	srv->Send(socket, replyMsg);
// 	return;
// }

// void COMMANDS::kick(Server *srv, int socket, const std::string &msg)
// {
// 	std::cout << "KICK\n";
// }

// void COMMANDS::join(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	bool isNewChannel = false;
// 	std::string replyMsg;

// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	std::string tok_Channel = *itToken;
// 	std::string tok_Passsword = "";
// 	if (++itToken != strTokens.end())
// 		tok_Passsword = *itToken;

// 	if (channels.find(tok_Channel) == channels.end())
// 	{
// 		Channel newChannel;
// 		newChannel.Password = tok_Passsword;
// 		newChannel.Modes = "";
// 		newChannel.Topic = "No Topic";
// 		channels[tok_Channel] = newChannel;
// 		isNewChannel = true;
// 	}
// 	else
// 	{
// 		if (channels[tok_Channel].Password != tok_Passsword)
// 		{
// 			replyMsg = ERR_PASSWDMISMATCH(srv->clients[socket].Nickname);
// 			srv->Send(socket, replyMsg);
// 			return;
// 		}
// 	}

// 	// set REPLY MSG and send it back to clientSocket
// 	replyMsg = RPL_JOINMSG(srv->clients[socket].Nickname, srv->clients[socket].Hostname, tok_Channel);

// 	// add channel to user
// 	srv->clients[socket].channel[tok_Channel].isJoined = true;

// 	std::map<int, Client>::iterator it;
// 	for (it = srv->clients.begin(); it != srv->clients.end(); it++)
// 	{
// 		if (it->second.channel[tok_Channel].isJoined == true)
// 			srv->Send(it->first, replyMsg);
// 	}

// 	if (isNewChannel)
// 	{
// 		srv->clients[socket].channel[tok_Channel].isOp = true;
// 		replyMsg = RPL_UMODEIS(srv->clients[socket].Hostname, tok_Channel, "+o", srv->clients[socket].Nickname);
// 		srv->Send(socket, replyMsg);
// 	}
// }

// void COMMANDS::nick(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	std::string clientNickname = *itToken;
// 	// set REPLY MSG and send it back to clientSocket
// 	std::string replyMsg = RPL_NICKCHANGE(srv->clients[socket].Nickname, clientNickname);
// 	std::map<int, Client>::iterator it;
// 	for (it = srv->clients.begin(); it != srv->clients.end(); it++)
// 		srv->Send(it->first, replyMsg);
// 	// overwrite old nickname
// 	srv->clients[socket].Nickname = clientNickname;
// }

// void COMMANDS::cap(Server *srv, int socket, const std::string &msg)
// {
// 	srv->clients[socket].recvMsg = "";
// 	std::string replyMsg;

// 	std::vector<std::string> strTokens = Helper::splitString(msg);
// 	std::vector<std::string>::iterator itToken = strTokens.begin();
// 	itToken++;
// 	if (*itToken == "LS")
// 	{
// 		replyMsg = "CAP * LS :multi-prefix sasl\r\n";
// 		srv->Send(socket, replyMsg);
// 		return;
// 	}
// 	if (*itToken == "REQ")
// 	{
// 		// substract all after "CAP REQ "
// 		itToken++;
// 		replyMsg = "CAP * ACK " + *itToken + CRLF;
// 		srv->Send(socket, replyMsg);
// 		return;
// 	}
// 	if (*itToken == "END")
// 	{
// 		std::string incomingMsg;
// 		// wait for receiving NICK <username and USER <username> <usermode> <hostname> <clientcomment>
// 		size_t breakit = 0;
// 		while (srv->Signal == false)
// 		{
// 			char buffer[1024];
// 			ssize_t bytes_received = recv(socket, buffer, sizeof(buffer), 0);
// 			if (bytes_received < 1)
// 			{
// 				// uncool solution ^^
// 				if (++breakit >= 99999999)
// 					break;
// 				continue;
// 			}

// 			buffer[bytes_received] = '\0';
// 			incomingMsg = std::string(buffer);
// 			break;
// 		}
// 		std::cout << magenta << "srvRecv: " << incomingMsg << res;

// 		// create tokens, split string on each space ' '.
// 		std::vector<std::string> strTokens = Helper::splitString(incomingMsg);
// 		std::vector<std::string>::iterator itToken = strTokens.begin();

// 		bool foundPASS = false;
// 		// set <password>
// 		if (itToken != strTokens.end() && *itToken == "PASS")
// 		{
// 			if (++itToken != strTokens.end() && !itToken->empty())
// 				itToken->erase(0, 1);
// 			if (*itToken == srv->getPassword())
// 			{
// 				foundPASS = true;
// 				srv->clients[socket].Password = *itToken;
// 			}
// 			++itToken;
// 		}
// 		if (!foundPASS)
// 		{
// 			replyMsg = ERR_PASSWDMISMATCH(srv->clients[socket].Nickname);
// 			srv->Send(socket, replyMsg);
// 			srv->clients[socket].Connected = false;
// 			return;
// 		}

// 		// set <nickname> and send reply to client
// 		if (itToken != strTokens.end() && *itToken == "NICK")
// 		{
// 			if (++itToken != strTokens.end())
// 				nick(srv, socket, "NICK " + *itToken + "\r\n");
// 			++itToken;
// 		}
// 		if (itToken != strTokens.end() && *itToken == "USER")
// 		{
// 			// set <username>
// 			if (++itToken != strTokens.end())
// 				srv->clients[socket].Username = *itToken;
// 			// set <usermode>
// 			if (++itToken != strTokens.end())
// 				srv->clients[socket].UserMode = *itToken;
// 			// set <hostname>
// 			if (++itToken != strTokens.end())
// 				srv->clients[socket].Hostname = *itToken;
// 			// set <realname>
// 			// skip ':1,8' (kvIRC comment)
// 			if (++itToken != strTokens.end() && !itToken->empty())
// 				itToken->erase(0, 5);
// 			while (itToken != strTokens.end())
// 			{
// 				srv->clients[socket].Realname += *itToken;
// 				if (++itToken != strTokens.end())
// 					srv->clients[socket].Realname += " ";
// 			}
// 		}

// 		// send welcome message and login
// 		replyMsg = RPL_WELCOME(srv->clients[socket].Nickname);
// 		srv->Send(socket, replyMsg);
// 		// send message of the day
// 		replyMsg = RPL_MOTD();
// 		srv->Send(socket, replyMsg);
// 	}
// }
