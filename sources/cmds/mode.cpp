#include "../../includes/Commands.hpp"

// MODE (Operator Command) - Change the channel’s mode:
// i: Set/remove Invite-only channel
// t: Set/remove the restrictions of the TOPIC command to channel operators
// k: Set/remove the channel key (password)
// o: Give/take channel operator privilege
// l: Set/remove the user limit to channel
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

	replyMsg = ": 421 " + clients[socket].Nickname + " MODE :Command is currently deactivated by Steven!" + CRLF;
	srv->Send(socket, replyMsg);
	return;
}
