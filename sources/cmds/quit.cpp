#include "../../includes/Commands.hpp"

void Commands::quit(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
	// substract whole message after 'QUIT :'
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	std::string quitMessage = *itToken;

	// prepare reply and set connected to false
	replyMsg = RPL_QUIT(clients[socket].Nickname, clients[socket].Username, clients[socket].Hostname, quitMessage);
	clients[socket].Connected = false;

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		srv->Send(itClient->first, replyMsg);
}