#include "../../includes/Commands.hpp"

void Commands::privmsg(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
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
	replyMsg = RPL_MESSAGE(clients[socket].Nickname, clients[socket].Username, clients[socket].Hostname, NickOrChannel, message);

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		if (itClient->first == socket && clients[socket].Nickname != NickOrChannel)
			continue;
		srv->Send(itClient->first, replyMsg);
	}
}