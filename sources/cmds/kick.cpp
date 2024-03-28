#include "../../includes/Commands.hpp"

void Commands::kick(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
	// create tokens, split string on each space ' '.
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;

	for (; itToken != strTokens.cend()--; itToken++)
		kickChannels.push_back(*itToken);

	std::string kickedUser = *itToken;
	std::string nickname = clients[socket].Nickname;
	std::string username = clients[socket].Username;
	std::string hostname = clients[socket].Hostname;

	for (itKickChannel = kickChannels.begin(); itKickChannel != kickChannels.cend(); itKickChannel++)
	{
		replyMsg = RPL_KICKUSER(nickname, username, hostname, *itKickChannel, kickedUser);
		srv->Send(socket, replyMsg);
	}
}
