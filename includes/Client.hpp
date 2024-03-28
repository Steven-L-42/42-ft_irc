#pragma once

#include <iostream>

class Client
{
private:
public:
	int socket;
	std::string Nickname;
	std::string Username;
	std::string Realname;
	std::string UserMode;
	std::string Hostname;
	std::string Password;
	std::string recvMsg;
	class Channel
	{
		public:
			bool isJoined;
			bool isOp;
	};
	std::map<std::string, Channel> channel;
	bool Connected;
};