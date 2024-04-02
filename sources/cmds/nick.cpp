#include "../../includes/Commands.hpp"

// nick is a command for everyone and help the user to change their nickname
void Commands::nick(int socket, const std::string &msg)
{
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	std::string newNickname = *itToken;
	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		if (itClient->second.Nickname == newNickname)
		{
			replyMsg = ERR_NICKNAMEINUSE(clients[socket].Nickname, newNickname);
			srv->Send(socket, replyMsg);
			return;
		}
	}

	// set REPLY MSG and send it back to clientSocket
	replyMsg = RPL_NICKCHANGE(clients[socket].Nickname, newNickname);

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		srv->Send(itClient->first, replyMsg);
	// overwrite old nickname
	clients[socket].Nickname = newNickname;
	clients[socket].Accepted = true;
}