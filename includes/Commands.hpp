#pragma once

#include <map>
#include "REPLYS.hpp"
#include "Helper.hpp"
#include "Server.hpp"
#include "Client.hpp"

//
class Server;
class Commands
{
private:
	Server *srv;
	std::map<int, Client> &clients;
	std::map<std::string, Channel> &channels;
	std::string replyMsg;
	std::vector<std::string> strTokens;
	std::vector<std::string>::iterator itToken;
	std::map<int, Client>::iterator itClient;
	std::vector<std::string> kickChannels;
	std::vector<std::string>::iterator itKickChannel;
	std::string channelName;

public:
	Commands(Server *_srv);
	~Commands();
	void cap(int socket, const std::string &msg);
	void nick(int socket, const std::string &msg);
	void join(int socket, const std::string &msg);
	void kick(int socket, const std::string &msg);
	void mode(int socket, const std::string &msg);
	void part(int socket, const std::string &msg);
	void ping(int socket, const std::string &msg);
	void privmsg(int socket, const std::string &msg);
	void quit(int socket, const std::string &msg);
	void topic(int socket, const std::string &msg);
	void unknown(int socket, const std::string &msg);
	void who(int socket, const std::string &msg);
	void whois(int socket, const std::string &msg);
};
