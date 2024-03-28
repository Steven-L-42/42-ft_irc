#include "../../includes/Commands.hpp"

void Commands::mode(int socket, const std::string &msg)
{
	(void)msg;
	// clients[socket].recvMsg = "";
	// // substract channel name without # and without \r\n
	// std::string substrCHANNEL = msg.substr(modeSize, msg.size() - modeSize - 2);
	//

	// // send Channel modes
	// replyMsg = RPL_CHANNELMODEIS(clients[socket].Nickname, substrCHANNEL, " ");
	// srv->Send(socket, replyMsg);

	clients[socket].recvMsg = "";
	replyMsg = ": 421 " + clients[socket].Nickname + " MODE :Command is currently deactivated by Steven!" + CRLF;
	srv->Send(socket, replyMsg);
	return;
}
