#include "../../includes/Commands.hpp"

void Commands::nick(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	std::string newNickname = *itToken;
	// set REPLY MSG and send it back to clientSocket
	replyMsg = RPL_NICKCHANGE(clients[socket].Nickname, newNickname);

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		srv->Send(itClient->first, replyMsg);
	// overwrite old nickname
	clients[socket].Nickname = newNickname;
}